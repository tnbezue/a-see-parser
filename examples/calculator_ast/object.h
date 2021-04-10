#ifndef __OBJECT_INCLUDED__
#define __OBJECT_INCLUDED__

#include <stdlib.h>

typedef void (*object_default_constructor_method)(void*);
typedef void (*object_destructor_method)(void*);
typedef void (*object_copy_ctor_method)(void*,const void*);
typedef void (*object_assign_method)(void*,const void*);
typedef int (*object_compare_method)(const void*,const void*);

typedef struct object_class_table_s object_class_table_t;

#define INSTANCE_HAS_CLASS_TABLE 1   // The instance of this object has pointer to class table
#define TRIVALLY_DESTRUCTIBLE    (1<<1)  // Basically, the destructor (and ancestor destructors) for this type don't do anything

struct object_class_table_s {
  object_class_table_t* parent_class_table_;
  const char* class_name_;
  unsigned int type_size_;
  int traits_;
  object_default_constructor_method default_ctor;
  object_destructor_method dtor;
  object_copy_ctor_method copy_ctor;
  object_assign_method assign;
  object_compare_method compare;
  int (*isa)(void*,const char*);
//  unsigned int class_name_hash_value_;
};

void object_default_ctor(void*);
void object_dtor(void*);
void object_copy_ctor(void*,const void*);
void object_assign(void*,const void*);
int object_compare(const void*,const void*);
int object_isa(void*,const char*);

extern object_class_table_t object_class_table;
// Microsoft C doesn't like empty structurs
typedef struct {
#ifdef _MSC_VER
  int __this_is_never_used__;
#endif
} object_instance_t;

typedef struct {
  const object_class_table_t* class_table_;
  object_instance_t;
} object_t;

/*
  Macro to create custom classes derived from object class above
    T -- new type  (without _t)
    P -- Parent type (without _t)
    CLASS_TABLE -- new entries to be added to this class table
    INSTANCE -- new entries to be added to this instance

  See simple_array and string for practical examples
*/
#define CLASS(T,P,CLASS_TABLE,INSTANCE) \
  typedef struct T ## _s T ## _t; \
  typedef struct T ## _class_table_s T ## _class_table_t; \
  typedef struct T ## _instance_s T ## _instance_t; \
  extern T ## _class_table_t T ## _class_table; \
  struct T ## _class_table_s { \
    P ## _class_table_t; \
    CLASS_TABLE \
  }; \
  struct T ## _instance_s {\
    P ## _instance_t; \
    INSTANCE \
  }; \
  struct T ## _s { \
    T ## _class_table_t *class_table_; \
    T ## _instance_t; \
  }

// Create object described by class table and assign class table member (if instance_has_class_table_)
// Should never use __NEWP_OBJECT and __NEW_OBJECT directly
#define __NEWP_OBJECT(CT,OBJ) ({ if(((object_class_table_t*)CT)->traits_ & INSTANCE_HAS_CLASS_TABLE) \
    ((object_t*)OBJ)->class_table_=(const object_class_table_t*)CT; })
#define __NEW_OBJECT(CT) ({ void* ptr=malloc(((object_class_table_t*)CT)->type_size_); \
    __NEWP_OBJECT(CT,ptr); ptr; })

/*object_t* this=malloc(((object_class_table_t*)ct)->type_size_);
  newp_object(ct,this);
  return this;
*/
/*
void newp_object(const void* ct,void* obj);
const object_class_table_t* this = ct;
  object_t* ptr=obj;
  if(this->instance_has_class_table_)
    ptr->class_table_ = this;
*/
//void* new_object(const void* ct);
// Sets class table (if instance_has_class_table_) of object than has already been allocated

// __NEW_xxxxx used when class table is a varariable

// placement new
//   CTP -- class table pointer
//   CTOR -- function pointer to constructuctor
//   ... -- arguments to constructor if any
#define __NEWP_CTOR(CTP,PTR,CTOR,...) (__NEWP_OBJECT(CTP,PTR),CTOR(PTR, ## __VA_ARGS__))
#define __NEWP(CTP,PTR) __NEWP_CTOR(CTP,PTR,((const object_class_table_t*)CTP)->default_ctor)
#define __NEWP_COPY(CTP,PTR,SRC) __NEWP_CTOR(CTP,PTR,((const object_class_table_t*)CTP)->copy_ctor,SRC)

// Allocate object and call specified constructor with provided arguemtns
#define __NEW_CTOR(CTP,CTOR,...) ({ \
  void * ptr = __NEW_OBJECT(CTP); \
  CTOR(ptr, ## __VA_ARGS__);\
  ptr;\
})
#define __NEW(CTP) __NEW_CTOR(CTP,((const object_class_table_t*)CTP)->default_ctor)
#define __NEW_COPY(CTP,SRC) __NEW_CTOR((CTP,((const object_class_table_t*)CTP)->copy_ctor,SRC)

// Allocate object and call specified constructor with provided arguemtns
//   CTP -- class table pointer
//   CTOR -- function pointer to constructuctor
//   ... -- arguments to constructor if any
#define NEW_CTOR(T,CTOR,...) (T ## _t *) __NEW_CTOR(&T ## _class_table,CTOR, ## __VA_ARGS__)
#define NEW_COPY(T,SRC) (T ## _t *) __NEW_COPY(&T ## _class_table,SRC)
#define NEW(T)  (T ## _t *) NEW_CTOR(T,T ## _class_table.default_ctor)
//#define NEW_POD(T) (T*) __NEW_CTOR(&T ## _class_table,object_default_ctor)
#define NEW_POD(T) (T*)malloc(sizeof(T))

#define NEWP_CTOR(T,PTR,CTOR,...) __NEW_CTOR(T## _class_table,PTR,CTOR, ## __VA_ARGS__)
#define NEWP(T,PTR) NEWP_CTOR(T ## _class_table,PTR,T ## _class_table.default_ctir)

void destroy_object(void*,const void*);
#define __DELETEP_OBJECT(ptr,CTP) destroy_object(ptr,CTP)
#define DELETEP_OBJECT(ptr,T) (__DELETEP_OBJECT(ptr,&T ## _class_table))
#define DELETEP(ptr) DELETEP_OBJECT(ptr,object)
#define __DELETE_OBJECT(ptr,CTP) (__DELETEP_OBJECT(ptr,CTP),free(ptr))
#define DELETE_OBJECT(ptr,T) __DELETE_OBJECT(ptr,&T ## _class_table)
#define DELETE(ptr) DELETE_OBJECT(ptr,object)
/*
  Calls the specified method using the pointer to the virtual table defined in this object
*/
#define __M(ptr,method,...) (ptr)->class_table_->method(ptr, ## __VA_ARGS__)
#define M(ptr,...) __M(ptr, __VA_ARGS__)

// Call parent method. Meant to be used inside of child method
// It may be better to use the method name -- parent_method(ptr,args)
// Especially if the parent method also calls a parent method using PM macro
#define __PM(ptr,method,...) (ptr)->class_table_->parent_class_table_->method(ptr, ## __VA_ARGS__)
#define PM(ptr,...) __PM(ptr, __VA_ARGS__)


#endif
