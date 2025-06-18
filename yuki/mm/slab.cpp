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
    caches[0] = createCache(8);
    caches[1] = createCache(16);
    caches[2] = createCache(32);
    kprintf(SLAB, "Caches located at 0x%lx\n", caches);
    caches[0].slabsEmpty = createSlab(caches[0].objSize, nullptr, nullptr);
    caches[1].slabsEmpty = createSlab(caches[1].objSize, nullptr, nullptr);
    caches[2].slabsEmpty = createSlab(caches[2].objSize, nullptr, nullptr);
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
    while (next != (uint64_t)newSlab) {
        *current = next;
        current = (uint64_t *)next;
        next += objSize;
    }
    newSlab->prev = prevSlab;
    newSlab->next = nextSlab;
    return newSlab;
}

void *kmalloc(size_t bytes) {
    uint64_t i;
    bool cacheExists = false;

    void *ret = caches[0].slabsEmpty->head;

    for (i = 0; i < 1024; i++) {
        if (caches[i].objSize == bytes) {
            cacheExists = true;
            kprintf(SLAB, "A cache for %d byte objects exists!\n", bytes);
            break;
        }
    } if (!cacheExists) {
        kprintf(SLAB, "A cache for %d byte objects does not exist! Creating new slab...\n", bytes);
        // TODO: Implement creation of new slabs
        return nullptr;
    }
    caches[i].slabsEmpty->head = (uint64_t *)*caches[i].slabsEmpty->head;
    kprintf(SLAB, "Head now points to 0x%lx\n", caches[i].slabsEmpty->head);
    return ret;
}

void kfree(void *ptr) {

}