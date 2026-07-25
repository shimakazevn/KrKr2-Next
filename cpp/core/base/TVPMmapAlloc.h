#ifndef TVP_MMAP_ALLOC_H
#define TVP_MMAP_ALLOC_H

#include <cstdlib>

inline void *TVPMmapAlloc(size_t size) {
    if(!size) return nullptr;
    void *ptr = nullptr;
    if(posix_memalign(&ptr, 16, size) != 0) return nullptr;
    return ptr;
}

inline void TVPMmapFree(void *mem) {
    if(mem) free(mem);
}

#define TVP_USE_MMAP_TEMP 1

#endif
