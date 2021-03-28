#include <stdlib.h>
#include <math.h>
#include "ast_tree.h"


//void dtor_noop(void* ptr) { } // for objects that do not require a destructor

//void ast_init(void* ptr,ast_dtor dtor,print_ast_element print,graph_ast_element graph,eval_ast_element eval)
void ast_init(void* ptr,const ast_vtable_t* vt)
{
  ast_t* this = ptr;
  this->vtable=vt;
}

ast_vtable_t unary_op_ast_vtable = { { unary_op_ast_dtor },unary_op_ast_print, unary_op_ast_graph, unary_op_ast_eval};

void unary_op_ast_ctor(void* ptr,int op,void* l)
{
  unary_op_ast_t* this = ptr;
  ast_init(ptr,&unary_op_ast_vtable);
  this->op=op;
  this->left=l;
}

void unary_op_ast_dtor(void* uo)
{
  const unary_op_ast_t* this = uo;
  DELETE(this->left);
}

void unary_op_ast_print(const void* uo,FILE* o)
{
  const unary_op_ast_t* this = uo;
  fprintf(o,"uop: %c\n",this->op);
  M(this->left,print,o);
}

void unary_op_ast_graph(const void* uo,FILE* o)
{
  const unary_op_ast_t* this = uo;
  M(this->left,graph,o);
  fprintf(o,"L%p [label= \"%c\"];\n",this,this->op);
  fprintf(o,"L%p -> L%p;\n",this,this->left);
}

double unary_op_ast_eval(const void* uo)
{
  const unary_op_ast_t* this = uo;
  double value = M(this->left,eval);
  if(this->op == '-')
    value = -value;
  return value;
}

ast_vtable_t binary_op_ast_vtable = {{binary_op_ast_dtor}, binary_op_ast_print,binary_op_ast_graph,binary_op_ast_eval };
void binary_op_ast_ctor(void*ptr,void* l,char op,void* r)
{
  binary_op_ast_t* this = ptr;
  ast_init(this,&binary_op_ast_vtable);
  this->left = l;
  this->op = op;
  this->right = r;
}

void binary_op_ast_dtor(void* bo)
{
  binary_op_ast_t* this = bo;
  DELETE(this->left);
  DELETE(this->right);
}

void binary_op_ast_print(const void* bo,FILE* o)
{
  const binary_op_ast_t*this = bo;
  M(this->left,print,o);
  fprintf(o,"bop: %c\n",this->op);
  M(this->right,print,o);
}

double binary_op_ast_eval(const void* bo)
{
  const binary_op_ast_t* this = bo;
  double lv = M(this->left,eval);
  double rv = M(this->right,eval);
  switch(this->op) {
    case '+':
      lv += rv;
      break;

    case '-':
      lv -= rv;
      break;

    case '*':
      lv *= rv;
      break;

    case '/':
      lv /= rv;
      break;

    case '^':
      lv = pow(lv,rv);
      break;
  }
  return lv;
}

void binary_op_ast_graph(const void* bo,FILE* o)
{
  const binary_op_ast_t* this = bo;
  M(this->left,graph,o);
  M(this->right,graph,o);
  fprintf(o,"L%p [label= \"%c\"];\n",this,this->op);
  fprintf(o,"L%p -> L%p;\n",this,this->left);
  fprintf(o,"L%p -> L%p;\n",this,this->right);
}

ast_vtable_t number_ast_vtable = {{number_ast_dtor},number_ast_print,number_ast_graph,number_ast_eval };
void number_ast_ctor(void*ptr,double v)
{
  number_ast_t* this = ptr;
  ast_init(this,&number_ast_vtable);
  this->value = v;
}

void number_ast_print(const void* n,FILE* o)
{
  const number_ast_t* this = n;
  fprintf(o,"%lg\n",this->value);
}

void number_ast_graph(const void* n,FILE* o)
{
  const number_ast_t* this = n;
  fprintf(o,"L%p [label= \"%lg\"];\n",this,this->value);
}

double number_ast_eval(const void* n)
{
  return ((const number_ast_t*)n)->value;
}

ast_vtable_t variable_ast_vtable = {{variable_ast_dtor},variable_ast_print,variable_ast_graph,variable_ast_eval };
void variable_ast_ctor(void* ptr,const variable_description_t* var)
{
  variable_ast_t* this= ptr;
  ast_init(this,&variable_ast_vtable);
  this->var = var;
}

void variable_ast_print(const void* v,FILE* o)
{
  const variable_ast_t* this = v;
  fprintf(o,"%s\n",this->var->name);
}

void variable_ast_graph(const void* v,FILE* o)
{
  const variable_ast_t* this = v;
  fprintf(o,"L%p [label= \"%s\"];\n",this,this->var->name);
}

double variable_ast_eval(const void* v)
{
  return *((const variable_ast_t*)v)->var->pvalue;
}

ast_vtable_t math_function_ast_vtable = { {math_function_ast_dtor},math_function_ast_print,
        math_function_ast_graph,math_function_ast_eval};
void math_function_ctor(void* ptr,const math_function_description_t* func,void* l)
{
  math_function_ast_t* this = ptr;
  ast_init(this,&math_function_ast_vtable);
  this->func=func;
  this->left=l;
}

void math_function_print(const void *mf,FILE* o)
{
  const math_function_ast_t* this = mf;
  fprintf(o,"%s\n",this->func->name);
  M(this->left,print,o);
}

void math_function_graph(const void*mf,FILE* o)
{
  const math_function_ast_t* this = mf;
  M(this->left,graph,o);
  fprintf(o,"L%p [label=\"%s\"];\n",this,this->func->name);
  fprintf(o,"L%p -> L%p;\n",this,this->left);
}

double math_function_eval(const void*mf)
{
  const math_function_ast_t* this = mf;
  return this->func->func(M(this->left,eval));
}
