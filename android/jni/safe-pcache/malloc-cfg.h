#ifndef WCDB_SAFE_PCACHE_MALLOC_CFG_H
#define WCDB_SAFE_PCACHE_MALLOC_CFG_H

#ifdef __cplusplus
#error This header is not intended to be included by anyone but dlmalloc's malloc.c
#endif

#include <sys/mman.h>

static inline void* mmap_impl(size_t sz) {
    void *ret = sys_mmap(NULL, sz, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
            -1, 0);
#ifdef __ANDROID__
    if (ret != MAP_FAILED) {
        sys_prctl(0x53564d41, 0, (uintptr_t) ret, sz, (uintptr_t) "NativeCrash_malloc");
    }
#endif
    return ret;
}

static inline int munmap_impl(void *addr, size_t sz) {
    return sys_munmap(addr, sz);
}

static inline void* mremap_impl(void *old_addr, size_t old_size, size_t new_size, int flags) {
    void *ret = sys_mremap(old_addr, old_size, new_size, flags);
#ifdef __ANDROID__
    if (ret != MAP_FAILED) {
        sys_prctl(0x53564d41, 0, (uintptr_t) ret, new_size, (uintptr_t) "NativeCrash_malloc");
    }
#endif
    return ret;
}

// Do not use sbrk as it will break other(system) malloc implementations
#define HAVE_MORECORE 0
#define MMAP(s)         mmap_impl((s))
#define MUNMAP(a, s)    munmap_impl((a), (s))
#define MREMAP(addr, osz, nsz, mv) mremap_impl((addr), (osz), (nsz), (mv))

// Thread-safe via spinlocks
#define USE_LOCKS 1
#define USE_SPIN_LOCKS 1


#endif //WCDB_SAFE_PCACHE_MALLOC_CFG_H
