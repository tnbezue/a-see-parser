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

CLASS(ast,object,
  void(*print) (const void*,FILE*);
  void(*graph) (const void*,FILE*);
  double (*eval) (const void*);
  ,
  );

#define ast_default_ctor object_default_ctor
#define ast_dtor object_dtor
#define ast_copy_ctor object_copy_ctor
#define ast_assign object_assign
#define ast_compare object_compare
CLASS(unary_op_ast,ast,
  ,
    int op;
    ast_t* left;
);

void unary_op_default_ctot(void*);
void unary_op_ast_ctor(void*,int o,void* r);
void unary_op_ast_dtor(void*);
#define unary_op_ast_copy_ctor object_copy_ctor
void unary_op_ast_assign(void*,const void*);
#define unary_op_ast_compare object_compare
void unary_op_ast_print(const void*,FILE*);
void unary_op_ast_graph(const void*,FILE*);
double unary_op_ast_eval(const void*);

CLASS(binary_op_ast,unary_op_ast,
,
  ast_t* right;
);

void binary_op_ast_default_ctor(void*);
void binary_op_ast_ctor(void*,void* l,char o,void* r);
void binary_op_ast_dtor(void*);
#define binary_op_ast_copy_ctor object_copy_ctor
void binary_op_ast_assign(void*,const void*);
#define binary_op_ast_compare object_compare
void binary_op_ast_print(const void*,FILE*);
void binary_op_ast_graph(const void*,FILE*);
double binary_op_ast_eval(const void*);

CLASS(number_ast,ast,
,
  double value;
);

#define number_ast_default_ctor object_default_ctor
void number_ast_ctor(void*,double);
#define number_ast_dtor object_dtor


void number_ast_print(const void*,FILE*);
void number_ast_graph(const void*,FILE*);
double number_ast_eval(const void*);
#define number_ast_copy_ctor object_copy_ctor
void number_ast_assign(void*,const void*);
#define number_ast_compare object_compare

CLASS(variable_ast,ast,
,
const variable_description_t* var;
);

#define variable_ast_default_ctor object_default_ctor
void variable_ast_ctor(void*,const variable_description_t*);
#define variable_ast_dtor object_dtor
#define variable_ast_copy_ctor object_copy_ctor
void variable_ast_assign(void*,const void*);
#define variable_ast_compare object_compare

void variable_ast_print(const void*,FILE*);
void variable_ast_graph(const void*,FILE*);
double variable_ast_eval(const void*);

CLASS(math_function_ast,unary_op_ast,
,
  const math_function_description_t* func;
);

void math_function_ast_default_ctor(void*);
void math_function_ast_ctor(void*,const math_function_description_t*,void*);
#define math_function_ast_dtor object_dtor
#define math_function_ast_copy_ctor object_copy_ctor
void math_function_ast_assign(void*,const void*);
#define math_function_ast_compare object_compare
void math_function_ast_print(const void*,FILE*);
void math_function_ast_graph(const void*,FILE*);
double math_function_ast_eval(const void*);


#endif
