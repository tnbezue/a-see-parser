#ifndef __UTILS_INCLUDED__
#define __UTILS_INCLUDED__

#include <stdlib.h>

#define NEW(T,CTOR,...) ({ T* ptr = malloc(sizeof(T)); CTOR(ptr, ## __VA_ARGS__); ptr; })
#define DELETE(ptr) (((base_t*)ptr)->vtable->dtor_(ptr), free(ptr))

void ctor_noop(void*);
typedef void (*dtor)(void*);
#define dtor_noop ctor_noop

typedef struct {
  dtor dtor_;
} base_vtable_t;

typedef struct {
  base_vtable_t* vtable;
} base_t;

#endif
