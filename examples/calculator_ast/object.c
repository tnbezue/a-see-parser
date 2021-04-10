#include "object.h"
#include <string.h>

void object_default_ctor(void * ptr) { }

void object_dtor(void * ptr) { }

// Should be same type. Maybe add check later
// All object derived from object can use this copy constructor
// Objects without class table (instance_has_class_table_=0) CAN NOT!!!!!!
void object_copy_ctor(void * d,const void* s)
{
  M((object_t*)d,default_ctor);
  M((object_t*)d,assign,s);
}

// Should be same type. Maybe add check later
void object_assign(void* d,const void* s)
{
  memcpy(d,s,((object_t*)d)->class_table_->type_size_);
}

// By default, all objects are equal
int object_compare(const void* lhs,const void* rhs)
{
  return 0;
}

int object_isa(void *ptr,const char* name)
{
  const object_class_table_t* this_class_table = ((object_t*)ptr)->class_table_;
  while(this_class_table) {
    if(strcmp(this_class_table->class_name_,name)==0)
      return 1;
    this_class_table=this_class_table->parent_class_table_;
  }
  return 0;
}

object_class_table_t object_class_table = {
  .parent_class_table_=NULL,
  .class_name_="OBJECT",
  .type_size_=sizeof(object_t),
  .traits_=INSTANCE_HAS_CLASS_TABLE|TRIVALLY_DESTRUCTIBLE,
  .default_ctor = object_default_ctor,
  .dtor = object_dtor,
  .copy_ctor = object_copy_ctor,
  .assign = object_assign,
  .compare = object_compare,
  .isa = object_isa
};

#if 0
void newp_object(const void* ct,void *obj)
{
  const object_class_table_t* this = ct;
  object_t* ptr=obj;
  if(this->instance_has_class_table_)
    ptr->class_table_ = this;
}

void* new_object(const void* ct)
{
  object_t* this=malloc(((object_class_table_t*)ct)->type_size_);
  newp_object(ct,this);
  return this;
}
#endif
/*
void destroyp_object(void* ptr,const void * ct)
{
  const object_class_table_t* this_class_table = ct;
  while(this_class_table) {
    this_class_table->dtor(ptr);
    this_class_table=this_class_table->parent_class_table_;
  }
}
*/
void destroy_object(void* ptr,const void* ct)
{
  const object_class_table_t* this_class_table=ct;
  if(this_class_table->traits_ & INSTANCE_HAS_CLASS_TABLE)
    this_class_table=((object_t*)ptr)->class_table_;
  while(this_class_table) {
    if(this_class_table->traits_ & TRIVALLY_DESTRUCTIBLE)
      break;
    this_class_table->dtor(ptr);
    this_class_table=this_class_table->parent_class_table_;
  }
}

#if 0
#define MULTIPLIER 37
unsigned int simple_hash(const char *str)
{
   unsigned int h;
   unsigned const char *p;

   h = 0;
   for (p = (unsigned const char*)str; *p != '\0'; p++)
      h = MULTIPLIER * h + *p;
   return h; // or, h % ARRAY_SIZE;
}
#endif
#if 0
void variant_ctor(void* ptr)
{
  variant_t* this=ptr;
  this->type = VAR_NONE;
  this->sv=NULL;
}

void variant_ctor_number(void* ptr,double value)
{
  variant_ctor(ptr);
  variant_set_number(ptr,value);
}

void variant_ctor_string(void*ptr,const char* str)
{
  variant_ctor(ptr);
  variant_set_string(ptr,str);
}

void variant_dtor(void* ptr)
{
  variant_t* this=ptr;
  if(this->type == VAR_STRING && this->sv) {
    free(this->sv);
  }
  this->type=VAR_NONE;
}

void variant_copy(void* d,const void* s)
{
  variant_t* dst = d;
  const variant_t* src = s;
  variant_dtor(dst);
  dst->type = src->type;
  if(dst->type == VAR_NONE)
    dst->sv = NULL;
  else if(dst->type == VAR_NUMBER)
    dst->rv = src->rv;
  else if(dst->type == VAR_STRING) {
    if(src->sv)
      dst->sv = strdup(src->sv);
    else
      dst->sv = NULL;
  }
}

int variant_cmp(const void* l,const void* r)
{
  const variant_t*lhs=l;
  const variant_t*rhs=r;
  int rc =0;
  if(lhs->type == rhs->type) {
    if(lhs->type == VAR_NUMBER)
      rc = (lhs->rv > rhs->rv) - (lhs->rv < rhs->rv);
    else
      rc = strcmp(lhs->sv,rhs->sv);
  }
  return rc;
}

void variant_set_number(void* ptr,double value)
{
  variant_dtor(ptr);
  variant_t* this=ptr;
  this->type=VAR_NUMBER;
  this->rv = value;
}

void variant_set_string(void* ptr,const char* str)
{
  variant_dtor(ptr);
  variant_t* this=ptr;
  this->type=VAR_STRING;
  this->sv = str ? strdup(str) : NULL;
}
#endif
