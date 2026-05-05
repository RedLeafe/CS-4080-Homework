#include <stdio.h>
#include <stdlib.h>

#include "memory.h"
#include "vm.h"

#ifdef DEBUG_LOG_GC
#include "debug.h"
#endif

#define GC_HEAP_GROW_FACTOR 2

static void freeObjectMemory(Obj* object);
extern void temporarilyRetainCompilerFunctions(void);
extern void undoTemporarilyRetainCompilerFunctions(void);


void* reallocate(void* pointer, size_t oldSize, size_t newSize) {
  vm.bytesAllocated += newSize - oldSize;
  if (newSize > oldSize) {
#ifdef DEBUG_STRESS_GC
    collectGarbage();
#endif
    if (vm.bytesAllocated > vm.nextGC) {
      collectGarbage();
    }
  }

  if (newSize == 0) {
    free(pointer);
    return NULL;
  }

  void* result = realloc(pointer, newSize);
  if (result == NULL) exit(1);
  return result;
}

void retainObj(Obj* obj) {
  if (obj != NULL) obj->refCount++;
}

void releaseObj(Obj* obj) {
  if (obj == NULL) return;
  obj->refCount--;
  if (obj->refCount > 0) return;

  Obj** slot = &vm.objects;
  while (*slot != NULL && *slot != obj) {
    slot = &(*slot)->next;
  }
  if (*slot != obj) return;
  *slot = obj->next;
  freeObjectMemory(obj);
}

static void sweepZeroRef(void) {
  Obj** slot = &vm.objects;
  while (*slot != NULL) {
    if ((*slot)->refCount == 0) {
      Obj* dead = *slot;
      *slot = dead->next;
      freeObjectMemory(dead);
    } else {
      slot = &(*slot)->next;
    }
  }
}

void collectGarbage(void) {
#ifdef DEBUG_LOG_GC
  printf("-- gc begin (refcount sweep)\n");
  size_t before = vm.bytesAllocated;
#endif

  temporarilyRetainCompilerFunctions();
  tableRemoveWhite(&vm.strings);
  sweepZeroRef();
  undoTemporarilyRetainCompilerFunctions();

  vm.nextGC = vm.bytesAllocated * GC_HEAP_GROW_FACTOR;

#ifdef DEBUG_LOG_GC
  printf("-- gc end\n");
  printf("   collected from %zu bytes, heap now %zu, next GC at %zu\n",
         before, vm.bytesAllocated, vm.nextGC);
#endif
}

static void freeObjectMemory(Obj* object) {
#ifdef DEBUG_LOG_GC
  printf("%p free type %d\n", (void*)object, object->type);
#endif

  switch (object->type) {
    case OBJ_CLOSURE: {
      ObjClosure* closure = (ObjClosure*)object;
      for (int i = 0; i < closure->upvalueCount; i++) {
        if (closure->upvalues[i] != NULL) {
          releaseObj((Obj*)closure->upvalues[i]);
        }
      }
      releaseObj((Obj*)closure->function);
      FREE_ARRAY(ObjUpvalue*, closure->upvalues, closure->upvalueCount);
      FREE(ObjClosure, object);
      break;
    }
    case OBJ_FUNCTION: {
      ObjFunction* function = (ObjFunction*)object;
      for (int i = 0; i < function->chunk.constants.count; i++) {
        Value constant = function->chunk.constants.values[i];
        if (IS_OBJ(constant)) releaseObj(AS_OBJ(constant));
      }
      freeChunk(&function->chunk);
      releaseObj((Obj*)function->name);
      FREE(ObjFunction, object);
      break;
    }
    case OBJ_NATIVE:
      FREE(ObjNative, object);
      break;
    case OBJ_UPVALUE:
      FREE(ObjUpvalue, object);
      break;
    case OBJ_STRING: {
      ObjString* string = (ObjString*)object;
      FREE_ARRAY(char, string->chars, string->length + 1);
      FREE(ObjString, object);
      break;
    }
  }
}

void freeObjects(void) {
  while (vm.objects != NULL) {
    sweepZeroRef();
    if (vm.objects == NULL) break;
    for (Obj* o = vm.objects; o != NULL; o = o->next) o->refCount = 0;
  }
}
