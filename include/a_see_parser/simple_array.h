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

#ifndef __SIMPLE_ARRAY_INCLUDED__
#define __SIMPLE_ARRAY_INCLUDED__

// Utility macros
#define NEW(T,CTOR,...) \
    ({ \
      T* ptr = malloc(sizeof(T));\
      CTOR(ptr,## __VA_ARGS__); \
      ptr; \
    })

#define DELETE(ptr,DTOR) DTOR(ptr),free(ptr)

void noop_ctor_dtor(void*);
/*
  Offers simple array operations
    * dynamic space allocation
    * Add to end
    * Remove from end
*/

/*
  Element operations
    * initialize (ctor)
    * destroy (dtor)
    * copy (copy)
*/
typedef void (*element_ctor)(void*);  // Method to initialize element (default ctor)
typedef void (*element_dtor)(void*);  // Method to destroy element (dtor)
typedef void (*element_copy)(void*,const void*); // Method to copy an element
typedef int (*element_cmp)(const void*,const void*); // Method to compare two elements

typedef unsigned int simple_array_size_t;
typedef struct {
  char* array;
  element_ctor default_ctor;
  element_dtor dtor;
  element_copy copy;
  element_cmp cmp;
  simple_array_size_t type_size;
  simple_array_size_t size;
  simple_array_size_t capacity;
} simple_array_t;

#define SIMPLE_ARRAY_INITIAL_CAPACITY 16
/*
  To avoid C compiler complaining about type mismatches, void pointers are used
*/
#define new_simple_array(element_size,element_ctor,element_dtor,element_copy,element_cmp) \
  NEW(simple_array_t,simple_array_ctor,element_size,element_ctor,element_dtor,element_copy,element_cmp)
#define delete_simple_array(a) DELETE(a,simple_array_dtor)
void simple_array_ctor(void*,simple_array_size_t,element_ctor,element_dtor,element_copy,element_cmp);
void simple_array_dtor(void*);

/*
  The following are needed in case a simple array has a simple array as elements
*/
void simple_array_default_ctor(void*); // In case simple array contains another simple array
void simple_array_copy(void*,const void*);
int simple_array_cmp(const void*,const void*);

void simple_array_increase_capacity(void*,simple_array_size_t);
void simple_array_push_back(void*,const void*);
void simple_array_pop_back(void*);
void* simple_array_find(void*,const void*,simple_array_size_t);
simple_array_size_t simple_array_size(const void*);
simple_array_size_t simple_array_capacity(const void*);
void simple_array_clear(void*);
void* simple_array_at(void*,simple_array_size_t);
const void* simple_array_cat(const void*,simple_array_size_t);
void* simple_array_begin(void*);
void* simple_array_end(void*);
const void* simple_array_cbegin(const void*);
const void* simple_array_cend(const void*);

#define ARRAY_OF_TYPE_DECLARATION(T) \
  typedef simple_array_t T ## _simple_array_t; \
  typedef T ## _t * T ## _simple_array_iterator; \
  typedef const T ## _t* T ## _simple_array_const_iterator; \
  T ## _simple_array_t * new_ ## T ## _simple_array(default_ctor,dtor,copy,cmp); \
  void delete_## T ## _array(T ## _array_t*); \
  void T ## _array_insert_range(T ## _array_t *,size_t,T ## _t*,size_t); \
  void T ## _array_pushback(T ## _array_t*,T ## _t*); \
  void T ## _array_popback(T ## _array_t*); \
  void T ## _array_remove_range(T ## _array_t*,size_t,size_t); \
  T ## _array_iterator T ## _array_find(T ## _array_t*,T ## _t*,size_t); \
  void T ## _array_sort(T ## _array_t*); \
  size_t T ## _array_size(const T ## _array_t*); \
  void T ## _array_insert(T ## _array_t*,size_t,T ## _t*); \
  void T ## _array_remove(T ## _array_t*,size_t); \
  void T ## _array_clear(T ## _array_t*); \
  T ## _array_iterator T ## _array_at(T ## _array_t*,size_t); \
  T ## _array_iterator T ## _array_begin(T ## _array_t*); \
  T ## _array_iterator T ## _array_end(T ## _array_t*); \
  T ## _array_const_iterator T ## _array_cbegin(const T ## _array_t*); \
  T ## _array_const_iterator T ## _array_cend(const T ## _array_t*);

#define ARRAY_OF_TYPE_IMPLEMENTATION(T) \
  T ## _array_t* new_ ## T ## _array(default_ctor dctor,dtor dtor,copy copy,cmp cmp,size_t is) \
      { return new_array(sizeof(T ## _t),dctor,dtor,copy,cmp,is); } \
  void delete_## T ## _array(T ## _array_t* a) { delete_array(a); } \
  void T ## _array_insert_range(T ## _array_t *a,size_t p,T ## _t*d,size_t s) { array_insert_range(a,p,d,s); } \
  void T ## _array_pushback(T ## _array_t*a,T ## _t*d) { array_pushback(a,d); } \
  void T ## _array_popback(T ## _array_t*a) { array_popback(a); } \
  void T ## _array_remove_range(T ## _array_t*a,size_t p,size_t n) { array_remove_range(a,p,n); } \
  T ## _array_iterator T ## _array_find(T ## _array_t*a,T ## _t* d,size_t p) { return array_find(a,d,p); } \
  void T ## _array_sort(T ## _array_t* a) { array_sort(a); } \
  size_t T ## _array_size(const T ## _array_t*a) { return array_size(a); } \
  void T ## _array_insert(T ## _array_t*a,size_t p,T ## _t*d) { array_insert(a,p,d); } \
  void T ## _array_remove(T ## _array_t*a,size_t p) { array_remove(a,p); } \
  void T ## _array_clear(T ## _array_t*a) {array_clear(a); } \
  T ## _array_iterator T ## _array_at(T ## _array_t*a,size_t p) { return array_at(a,p); } \
  T ## _array_iterator T ## _array_begin(T ## _array_t*a) { return array_begin(a); } \
  T ## _array_iterator T ## _array_end(T ## _array_t*a) { return array_end(a); } \
  T ## _array_const_iterator T ## _array_cbegin(const T ## _array_t*a) { return array_cbegin(a); } \
  T ## _array_const_iterator T ## _array_cend(const T ## _array_t*a) { return array_cend(a); }

#endif
