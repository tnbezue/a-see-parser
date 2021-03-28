#ifndef __AST_TREE_INCLUDED__
#define __AST_TREE_INCLUDED__

// Abstract syntax tree

#include <stdio.h>
#include <stddef.h>
#include "object.h"

typedef double (*math_function)(double);
typedef struct {
  const char* name;
  math_function func;
} math_function_description_t;

typedef struct  {
  const char* name;
  double *pvalue;
} variable_description_t;

// use void* to avoid compiler warnings
typedef void (*ast_dtor)(void*);
typedef void (*print_ast_element)(const void*,FILE*);
typedef void (*graph_ast_element)(const void*,FILE*);
typedef double (*eval_ast_element)(const void*);

typedef struct {
  object_vtable_t;
  print_ast_element print;
  graph_ast_element graph;
  eval_ast_element eval;
} ast_vtable_t;

#ifndef _MSC_VER
typedef struct
{
} ast_instance_t;
#endif
typedef struct {
  const ast_vtable_t* vtable;
#ifndef _MSC_VER
  ast_instance_t;
#endif
} ast_t;

void ast_init(void*,const ast_vtable_t*);

typedef struct {
  ast_vtable_t;
} unary_op_ast_vtable_t;

typedef struct {
#ifndef _MSC_VER
  ast_instance_t;
#endif
  ast_t* left;
  int op;
} unary_op_ast_instance_t;

typedef struct {
  unary_op_ast_vtable_t *vtable;
  unary_op_ast_instance_t;
} unary_op_ast_t;

void unary_op_ast_ctor(void*,int o,void* r);
void unary_op_ast_dtor(void*);
void unary_op_ast_print(const void*,FILE*);
void unary_op_ast_graph(const void*,FILE*);
double unary_op_ast_eval(const void*);

typedef struct {
  unary_op_ast_vtable_t;
} binary_op_ast_vtable_t;

typedef struct {
  unary_op_ast_instance_t;
  ast_t* right;
} binary_op_ast_instance_t;

typedef struct {
  binary_op_ast_vtable_t *vtable;
  binary_op_ast_instance_t;
} binary_op_ast_t;

void binary_op_ast_ctor(void*,void* l,char o,void* r);
void binary_op_ast_dtor(void*);
void binary_op_ast_print(const void*,FILE*);
void binary_op_ast_graph(const void*,FILE*);
double binary_op_ast_eval(const void*);

typedef struct {
  ast_vtable_t;
} number_ast_vtable_t;

typedef struct {
#ifndef _MSC_VER
  ast_instance_t;
#endif
  double value;
}number_ast_instance_t;

typedef struct {
  number_ast_vtable_t* vtable;
  number_ast_instance_t;
} number_ast_t;

void number_ast_ctor(void*,double);
#define number_ast_dtor dtor_noop
void number_ast_print(const void*,FILE*);
void number_ast_graph(const void*,FILE*);
double number_ast_eval(const void*);

typedef struct {
  ast_vtable_t;
} variable_ast_vtable_t;

typedef struct {
#ifndef _MSC_VER
  ast_instance_t;
#endif
  const variable_description_t* var;
} variable_ast_instance_t;

typedef struct {
  variable_ast_vtable_t* vtable;
  variable_ast_instance_t;
} variable_ast_t;

void variable_ast_ctor(void*,const variable_description_t*);
#define variable_ast_dtor dtor_noop
void variable_ast_print(const void*,FILE*);
void variable_ast_graph(const void*,FILE*);
double variable_ast_eval(const void*);

typedef struct {
  unary_op_ast_vtable_t;
} math_function_ast_vtable_t;

typedef struct {
  unary_op_ast_instance_t;
  const math_function_description_t* func;
} math_function_ast_instance_t;

typedef struct {
  math_function_ast_vtable_t* vtable;
  math_function_ast_instance_t;
} math_function_ast_t;

void math_function_ast_ctor(void*,const math_function_description_t*,void*);
#define math_function_ast_dtor unary_op_ast_dtor
void math_function_ast_print(const void*,FILE*);
void math_function_ast_graph(const void*,FILE*);
double math_function_ast_eval(const void*);


#endif
