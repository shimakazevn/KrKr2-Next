#ifndef TVPDecodeArenaH
#define TVPDecodeArenaH

#include <cstddef>

inline bool TVPDecodeArenaActive() { return false; }
inline void *TVPDecodeArenaAlloc(size_t) { return nullptr; }
inline size_t TVPDecodeArenaLastPeak() { return 0; }
inline size_t TVPDecodeArenaLastCount() { return 0; }

#endif
