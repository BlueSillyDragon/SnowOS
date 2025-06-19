#include <cstddef>
#include <cstdint>

struct Slab {
    Slab *prev;
    uint64_t refCount;
    uint64_t *head;
    Slab *next;
};

struct SlabCache {
    uint64_t objSize;
    Slab *slabsEmpty;
    Slab *slabsPartial;
    Slab *slabsFull;
};

void initSlab(uint64_t hhdm);
SlabCache createCache(uint64_t objSize);
Slab *createSlab(uint64_t objSize, Slab *prevSlab, Slab *nextSlab);
Slab *getLastSlab(SlabCache cache);
void *kmalloc(size_t bytes);
void kfree(void *ptr);