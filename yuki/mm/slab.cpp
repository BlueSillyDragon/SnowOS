#include <cstddef>
#include <cstdint>
#include <inc/klibc/string.hpp>
#include <inc/io/kprintf.hpp>
#include <inc/mm/slab.hpp>
#include <inc/mm/pmm.hpp>
#include <inc/mm/vmm.hpp>

void *next = nullptr;
uint64_t hhdmOffs;

SlabCache *caches;

void initSlab(uint64_t hhdm) {
    kprintf(SLAB, "Initializing Slab Allocator...\n");
    hhdmOffs = hhdm;
    caches = reinterpret_cast<SlabCache *>(pmmAlloc() + hhdm);
    memset(caches, 0, 0x1000);

    uint64_t objSize = 8;
    for (uint64_t i = 0; objSize <= 512; i++) {
        caches[i] = createCache(objSize);
        objSize *= 2;
    }

    for (uint64_t i = 0; caches[i].objSize != 0; i++) {
        caches[i].slabsEmpty = createSlab(caches[i].objSize, nullptr, nullptr);
    }

    kprintf(SLAB, "Caches located at 0x%lx\n", caches);
    kprintf(SLAB, "Slab Allocator Initialized!\n");
}

SlabCache createCache(uint64_t objSize) {
    SlabCache newCache = {.objSize = objSize, .slabsEmpty = nullptr, .slabsPartial = nullptr, .slabsFull = nullptr};
    kprintf(SLAB, "New cache created that stores %d byte objects\n", newCache.objSize);
    return newCache;
}

// TODO: Add metadata so that we don't need the size passed to kfree()
Slab *createSlab(uint64_t objSize, Slab *prevSlab, Slab *nextSlab) {
    Slab *newSlab = reinterpret_cast<Slab *>((pmmAlloc() + 0x1000 - sizeof(Slab)) + hhdmOffs);
    newSlab->head = reinterpret_cast<uint64_t *>((uint64_t)newSlab & ~0xfff);

    uint64_t next = (uint64_t)newSlab->head + objSize;
    uint64_t *current = newSlab->head;

    while (next < (uint64_t)newSlab) {
        *current = next;
        current = (uint64_t *)next;
        next += objSize;
    }
    *current = NULL;
    
    newSlab->prev = prevSlab;
    newSlab->next = nextSlab;
    newSlab->refCount = 0;
    return newSlab;
}

Slab *getSlab(SlabCache cache) {
    if (cache.slabsPartial == nullptr) {
        if (cache.slabsEmpty == nullptr) {
            cache.slabsEmpty = createSlab(cache.objSize, nullptr, nullptr);
        }
        return cache.slabsEmpty;
    }
    return cache.slabsPartial;
}

Slab *findSlab(SlabCache cache, Slab *slabToFind, SLAB_TYPE type) {
    Slab *currentSlab =  nullptr;
    switch (type) {
        case EMPTY:
            currentSlab = cache.slabsEmpty;
            break;
        case PARTIAL:
            currentSlab = cache.slabsPartial;
            break;
        case FULL:
            currentSlab = cache.slabsFull;
            break;
    }
    while (currentSlab != nullptr) {
            if (currentSlab == slabToFind) {
                return currentSlab;
            }
            currentSlab = currentSlab->next;
    }
    return nullptr;
}

void *kmalloc(size_t bytes) {
    uint64_t i;
    bool cacheExists = false;

    for (i = 0; i < 1024; i++) {
        if (caches[i].objSize == bytes) {
            cacheExists = true;
            break;
        }
    } if (!cacheExists) {
        for (i = 0; caches[i].objSize != 0; i++) {}

        kprintf(SLAB, "First empty is cache %d at 0x%lx\n", i, &caches[i]);

        caches[i] = createCache(bytes);
        caches[i].slabsEmpty = createSlab(bytes, nullptr, nullptr);
    }

    Slab *currentSlab = getSlab(caches[i]);

    while (currentSlab->head == nullptr) {
        // First, move this slab to the full list
        caches[i].slabsPartial = currentSlab->next;
        currentSlab->next = caches[i].slabsFull;
        caches[i].slabsFull = currentSlab;

        currentSlab = getSlab(caches[i]);
    }

    void *ret = currentSlab->head;

    currentSlab->head = (uint64_t *)*currentSlab->head;
    currentSlab->refCount++;

    // Make sure not to put the currentSlab on the freelist when it's already on it
    if (findSlab(caches[i], currentSlab, PARTIAL) != currentSlab) {
        currentSlab->next = caches[i].slabsPartial;
        caches[i].slabsPartial = currentSlab;

        if (findSlab(caches[i], currentSlab, EMPTY) == currentSlab) {
            caches[i].slabsEmpty = currentSlab->next;
        }
    }

    return ret;
}

void kfree(void *ptr, size_t bytes) {
    uint64_t i;
    Slab *slab = reinterpret_cast<Slab *>(((uint64_t)ptr & ~0xfff) + 0x1000 - sizeof(Slab));

    uint64_t *newHead = (uint64_t *)ptr;
    *newHead = (uint64_t)slab->head;
    slab->head = newHead;
    slab->refCount--;

    for (i = 0; i < 1024; i++) {
        if (caches[i].objSize == bytes) {
            break;
        }
    }

    if (slab->refCount == 0) {
        kprintf(SLAB, "Freed last object of slab, moving to empty list...\n");

        slab->next = caches[i].slabsEmpty;
        caches[i].slabsEmpty = slab;

        // Remove off the partial list (this'll break if freeing from a free slab, but just, don't do that)
        caches[i].slabsPartial = slab->next;

        slab->prev = nullptr;

    } else if (findSlab(caches[i], slab, FULL) != nullptr) {
        kprintf(SLAB, "Moving to partial list\n");
        
        // When it's on the top
        if (caches[i].slabsFull == slab) {
            caches[i].slabsFull = slab->next;
            if (slab->next != nullptr) {
                slab->next->prev = nullptr;
            }
        } else {
            slab->prev->next = slab->next;
            if (slab->next != nullptr) {
                slab->next->prev = nullptr;
            }
        }

        slab->next = caches[i].slabsPartial;
        caches[i].slabsPartial = slab;

        slab->prev = nullptr;
    }

    kprintf(SLAB, "Object of %d bytes was freed!\n", bytes);
}