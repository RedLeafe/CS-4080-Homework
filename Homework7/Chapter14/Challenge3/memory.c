#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"

// The book suggests allocating a single big block once at startup.
// This size needs to be large enough for the programs you run.
#ifndef CLOX_HARDCORE_HEAP_SIZE
#define CLOX_HARDCORE_HEAP_SIZE (8 * 1024 * 1024) // 8 MiB
#endif

static uint8_t* heapStart = NULL;
static uint8_t* heapNext = NULL;
static uint8_t* heapEnd = NULL;

void initMemory(void) {
  if (heapStart != NULL) return; // idempotent

  heapStart = (uint8_t*)malloc(CLOX_HARDCORE_HEAP_SIZE);
  if (heapStart == NULL) {
    fprintf(stderr, "Hardcore heap allocation failed\n");
    exit(1);
  }

  heapNext = heapStart;
  heapEnd = heapStart + CLOX_HARDCORE_HEAP_SIZE;
}

// Minimal allocator:
// - new blocks are carved via a bump pointer
// - shrinking returns the same pointer (no real shrinking)
// - freeing does not actually reclaim memory (program exits, OS reclaims)
void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  if (newSize == 0) {
    // Can't call free() in hardcore mode; we just "forget" this allocation.
    return NULL;
  }

  if (heapStart == NULL) {
    fprintf(stderr, "Hardcore heap is not initialized\n");
    exit(1);
  }

  if (pointer == NULL) {
    // Equivalent to malloc(newSize) in the book's reallocate() contract.
    size_t allocationSize = newSize;
    if (heapNext + allocationSize > heapEnd) {
      fprintf(stderr, "Hardcore heap exhausted\n");
      exit(1);
    }

    void* result = heapNext;
    heapNext += allocationSize;
    return result;
  }

  if (newSize <= oldSize) {
    // Shrink in-place.
    return pointer;
  }

  // Grow: allocate a new block and copy old contents.
  size_t allocationSize = newSize;
  if (heapNext + allocationSize > heapEnd) {
    fprintf(stderr, "Hardcore heap exhausted\n");
    exit(1);
  }

  void* result = heapNext;
  heapNext += allocationSize;

  memcpy(result, pointer, oldSize);
  return result;
}

