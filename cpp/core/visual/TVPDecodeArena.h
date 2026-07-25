#ifndef TVPDecodeArenaH
#define TVPDecodeArenaH

#include <cstddef>

class TVPDecodeArena {
public:
    void Begin() {}
    void End() {}
    size_t GetLastPeakBytes() const { return 0; }
    size_t GetLastAllocCount() const { return 0; }
    bool IsActive() const { return false; }
    void *Alloc(size_t) { return nullptr; }
    static TVPDecodeArena &Instance() {
        static TVPDecodeArena arena;
        return arena;
    }
};

inline bool TVPDecodeArenaActive() { return false; }
inline void *TVPDecodeArenaAlloc(size_t) { return nullptr; }
inline size_t TVPDecodeArenaLastPeak() { return 0; }
inline size_t TVPDecodeArenaLastCount() { return 0; }

#endif
