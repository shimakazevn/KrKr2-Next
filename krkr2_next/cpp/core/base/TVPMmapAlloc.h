#ifndef TVP_MMAP_ALLOC_H
#define TVP_MMAP_ALLOC_H

#include <cstdlib>

inline void *TVPMmapAlloc(size_t size) {
    if(!size) return nullptr;
    return malloc(size);
}

inline void TVPMmapFree(void *mem) {
    if(mem) free(mem);
}

// TVP_USE_MMAP_TEMP disabled in favor of standard C++ malloc/free/delete[]
// #define TVP_USE_MMAP_TEMP 1

#endif
