#ifndef __PARSER_INCLUDED__
#define __PARSER_INCLUDED__

// Abstract syntax tree

#include <stdio.h>
#include <stddef.h>
#include "utils.h"

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
  base_vtable_t;
  print_ast_element print;
  graph_ast_element graph;
  eval_ast_element eval;
} ast_vtable_t;

typedef struct {
  const ast_vtable_t* vtable;
} ast_t;

//void ast_init(void*,ast_dtor,print_ast_element,graph_ast_element,eval_ast_element);
void ast_init(void*,const ast_vtable_t*);
//#define delete_ast(p) (((ast_t*)p)->vtable->dtor(p),free(p))

typedef struct {
  ast_t;
  ast_t* left;
  int op;
} unary_op_t;

void unary_op_ctor(void*,int o,void* r);
void unary_op_dtor(void*);
void unary_op_print(const void*,FILE*);
void unary_op_graph(const void*,FILE*);
double unary_op_eval(const void*);

typedef struct {
  unary_op_t;
  ast_t* right;
} binary_op_t;

void binary_op_ctor(void*,void* l,char o,void* r);
void binary_op_dtor(void*);
void binary_op_print(const void*,FILE*);
void binary_op_graph(const void*,FILE*);
double binary_op_eval(const void*);

typedef struct {
  ast_t;
  double value;
} number_t;
void number_ctor(number_t*,double);
#define number_dtor dtor_noop
void number_print(const void*,FILE*);
void number_graph(const void*,FILE*);
double number_eval(const void*);

typedef struct {
  ast_t;
  const variable_description_t* var;
} variable_t;
void variable_ctor(variable_t*,const variable_description_t*);
#define variable_dtor dtor_noop
void variable_print(const void*,FILE*);
void variable_graph(const void*,FILE*);
double variable_eval(const void*);

typedef struct {
  unary_op_t;
  const math_function_description_t* func;
} math_function_t;

void math_function_ctor(math_function_t*,const math_function_description_t*,void*);
#define math_function_dtor unary_op_dtor
void math_function_print(const void*,FILE*);
void math_function_graph(const void*,FILE*);
double math_function_eval(const void*);


#endif
