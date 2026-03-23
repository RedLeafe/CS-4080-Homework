#ifndef CLOX_MEMORY_H
#define CLOX_MEMORY_H

#include "common.h"

// Dynamic array growth strategy (amortized O(1) append), matching clox's normal
// `memory.h` interface. Even in hardcore mode, we still want these macros.
#define GROW_CAPACITY(capacity) \
  ((capacity) < 8 ? 8 : (capacity) * 2)

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
  (type*)reallocate(pointer, sizeof(type) * (oldCount), \
                     sizeof(type) * (newCount))

#define FREE_ARRAY(type, pointer, oldCount) \
  reallocate(pointer, sizeof(type) * (oldCount), 0)

// Initializes the single "hardcore" heap by calling malloc() exactly once.
// Must be called before any clox allocation.
void initMemory(void);

// Hardcore-mode implementation of clox's reallocate() contract.
// Note: This deliberately does NOT call realloc() / free() internally.
void* reallocate(void* pointer, size_t oldSize, size_t newSize);

#endif

