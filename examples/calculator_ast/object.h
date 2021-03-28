#ifndef __OBJECT_INCLUDED__
#define __OBJECT_INCLUDED__

#include <stdlib.h>

// T is TYPE, CTOR is the constructor VA_ARGS is args to constructor
#define NEW(T,CTOR,...) ({ T* ptr = malloc(sizeof(T)); CTOR(ptr, ## __VA_ARGS__); ptr; })
#define DELETE(ptr) (((object_t*)ptr)->vtable->dtor_(ptr), free(ptr))

void ctor_noop(void*);
typedef void (*dtor)(void*);
#define dtor_noop ctor_noop

typedef struct {
  dtor dtor_;
} object_vtable_t;

// Microsoft C doesn't like empty structurs
#ifndef _MSC_VER
typedef struct {
} object_instance_t;
#endif

typedef struct {
  object_vtable_t* vtable;
#ifndef _MSC_VER
  object_instance_t;
#endif
} object_t;

/*
  Calls the specified method using the pointer to the virtual table defined in this object
*/
#define __M(ptr,method,...) (ptr)->vtable->method(ptr, ## __VA_ARGS__)
#define M(ptr,...) __M(ptr, __VA_ARGS__)

#endif
