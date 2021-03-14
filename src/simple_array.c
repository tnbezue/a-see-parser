/*
		Copyright (C) 2021  by Terry N Bezue

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <a_see_parser/simple_array.h>

void noop_ctor_dtor(void* ptr) { }
/*
void* new_simple_array(simple_array_size_t type_size,element_ctor ctor,element_dtor dtor,element_copy copy,element_cmp cmp)
{
  simple_array_t* this = malloc(sizeof(simple_array_t));
  simple_array_ctor(this,type_size,ctor,dtor,copy,cmp);
  return this;
}
*/
void simple_array_ctor(void* sa,simple_array_size_t type_size,element_ctor ctor,element_dtor dtor,element_copy copy,element_cmp cmp)
{
  simple_array_t* this = sa;
  this->array = NULL;
  this->default_ctor = ctor;
  this->dtor = dtor;
  this->copy = copy;
  this->cmp = cmp;
  this->type_size = type_size;
  this->size = 0;
  this->capacity = 0;
}

void simple_array_dtor(void* sa)
{
  simple_array_t* this = sa;
  if(this->array) {
    simple_array_clear(this);
    free(this->array);
  }
}

void simple_array_increase_capacity(void* sa,simple_array_size_t n)
{
  simple_array_t* this = sa;
  simple_array_size_t capacity_needed = this->size + n;
  if(capacity_needed > this->capacity) {
    if(this->capacity == 0) this->capacity = SIMPLE_ARRAY_INITIAL_CAPACITY;
    while(this->capacity < capacity_needed) this->capacity <<= 1;
    this->array = realloc(this->array,this->capacity*this->type_size);
  }
}

simple_array_size_t simple_array_size(const void* sa)
{
  return sa ? ((const simple_array_t*)sa)->size : 0;
}

simple_array_size_t simple_array_capacity(const void* sa)
{
  return sa ? ((const simple_array_t*)sa)->capacity : 0;
}

void* simple_array_at(void* sa,simple_array_size_t pos)
{
  simple_array_t* this=sa;
  return this->array + this->type_size*pos;
}

const void* simple_array_cat(const void* sa,simple_array_size_t pos)
{
  const simple_array_t* this=sa;
  return this->array + this->type_size*pos;
}

void simple_array_push_back(void* sa,const void* data)
{
  simple_array_t* this = sa;
  simple_array_increase_capacity(this,1);
  char* ptr = simple_array_at(this,this->size);
  if(this->copy) {
    if(this->default_ctor)
      this->default_ctor(ptr);
    else
      memset(ptr,0,this->type_size);
    this->copy(ptr,data);
  } else
    memcpy(ptr,data,this->type_size);
  this->size++;
}

void simple_array_pop_back(void* sa)
{
  simple_array_t* this = sa;
  if(this->size > 0) {
    this->size--;
    if(this->dtor)
      this->dtor(simple_array_at(this,this->size));
  }
}

void* simple_array_find(void* sa, const void* data,simple_array_size_t pos)
{
  simple_array_t* this = sa;
  char* ret = simple_array_at(this,pos);
  if(this->cmp) {
    int i;
    for(i=pos;i<this->size;i++,ret+=this->type_size) {
      if(this->cmp(ret,data)==0)
        break;
    }
  }
  return ret;
}

void* simple_array_begin(void* sa)
{
  return ((simple_array_t*)sa)->array;
}

void* simple_array_end(void* sa)
{
  return simple_array_at(sa,((simple_array_t*)sa)->size);
}

const void* simple_array_cbegin(const void* sa)
{
  return ((const simple_array_t*)sa)->array;
}

const void* simple_array_cend(const void* sa)
{
  return simple_array_cat(sa,((const simple_array_t*)sa)->size);
}

void simple_array_clear(void* sa)
{
  simple_array_t* this = sa;
  if(this->size > 0) {
    if(this->dtor) {
      int i;
      char* ptr = this->array;
      for(i=0;i<this->size;i++,ptr+=this->type_size) {
        this->dtor(ptr);
      }
    }
    this->size=0;
  }
}

void simple_array_default_ctor(void* sa)
{
  memset(sa,0,sizeof(simple_array_t));
  ((simple_array_t*)sa)->type_size=1;
}

void simple_array_copy(void* d,const void* s)
{
  simple_array_t* dst = d;
  const simple_array_t* src = s;
  simple_array_dtor(dst);
  simple_array_ctor(dst,src->type_size,src->default_ctor,src->dtor,src->copy,src->cmp);
  simple_array_increase_capacity(dst,src->size);
  if(dst->copy) {
    int i;
    char* ptr;
    if(dst->default_ctor) {
      for(i=0,ptr = dst->array;i<src->size;i++,ptr+=dst->type_size)
        dst->default_ctor(ptr);
    } else {
      memset(dst->array,0,src->size*dst->type_size);
    }
    const char* sptr;
    for(i=0,ptr=dst->array,sptr=src->array;i<src->size;i++,ptr+=dst->type_size,sptr+=src->type_size)
      dst->copy(ptr,sptr);
  } else {
    memcpy(dst->array,src->array,dst->type_size*src->size);
  }
  dst->size=src->size;
}
