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
    for (uint64_t i = 0; objSize <= 256; i++) {
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
    return newSlab;
}

Slab *getLastSlab(SlabCache cache) {
    if (cache.slabsEmpty == nullptr) {
        kprintf(SLAB, "No empty slabs available!\n");
        return nullptr;
    }

    Slab *currentSlab = cache.slabsEmpty;

    uint64_t slabCount = 1;

    while (currentSlab->next != nullptr) {
        currentSlab = currentSlab->next;
        slabCount++;
    }

    //kprintf(SLAB, "There were %d slabs located in %d byte cache\n", slabCount, cache.objSize);
    return currentSlab;
}

void *kmalloc(size_t bytes) {
    uint64_t i;
    bool cacheExists = false;

    for (i = 0; i < 1024; i++) {
        if (caches[i].objSize == bytes) {
            cacheExists = true;
            //kprintf(SLAB, "A cache for %d byte objects exists!\n", bytes);
            break;
        }
    } if (!cacheExists) {
        //kprintf(SLAB, "A cache for %d byte objects does not exist! Creating new slab...\n", bytes);
        // TODO: Implement creation of new slabs
        return nullptr;
    }

    Slab *currentSlab = getLastSlab(caches[i]);

    if (currentSlab->head == nullptr) {
        if (currentSlab->prev == nullptr || currentSlab->prev->head == nullptr) {
            currentSlab->next = createSlab(caches[i].objSize, currentSlab, nullptr);
            currentSlab = currentSlab->next;
        } else {
            currentSlab = currentSlab->prev;
        }
    }

    void *ret = currentSlab->head;

    currentSlab->head = (uint64_t *)*currentSlab->head;
    currentSlab->refCount++;
    return ret;
}

void kfree(void *ptr) {
    Slab *slab = reinterpret_cast<Slab *>(((uint64_t)ptr & ~0xfff) + 0x1000 - sizeof(Slab));
    uint64_t *newHead = (uint64_t *)ptr;
    *newHead = (uint64_t)slab->head;
    slab->head = newHead;
    slab->refCount--;
}