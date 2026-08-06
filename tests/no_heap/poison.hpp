#pragma once

#include <cstddef>

// Intentionally remap common heap APIs so strict no-heap checks fail fast.
extern "C" {
void *cljonic_forbidden_malloc(std::size_t);
void *cljonic_forbidden_calloc(std::size_t, std::size_t);
void *cljonic_forbidden_realloc(void *, std::size_t);
void cljonic_forbidden_free(void *);
void *cljonic_forbidden_aligned_alloc(std::size_t, std::size_t);
int cljonic_forbidden_posix_memalign(void **, std::size_t, std::size_t);
}

#define malloc cljonic_forbidden_malloc
#define calloc cljonic_forbidden_calloc
#define realloc cljonic_forbidden_realloc
#define free cljonic_forbidden_free
#define aligned_alloc cljonic_forbidden_aligned_alloc
#define posix_memalign cljonic_forbidden_posix_memalign

// Optional aggressive C++ poison names for dedicated experiments.
// Disabled by default because keyword poisoning breaks standard headers.
#ifdef CLJONIC_ENABLE_AGGRESSIVE_CPP_HEAP_POISON
#define make_unique CLJONIC_FORBIDDEN_make_unique
#define make_shared CLJONIC_FORBIDDEN_make_shared
#endif
