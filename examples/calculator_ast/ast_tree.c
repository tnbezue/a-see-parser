#include <stdlib.h>
#include <math.h>
#include "ast_tree.h"


void dtor_noop(void* ptr) { } // for objects that do not require a destructor

#define NEW(T) (T*)malloc(sizeof(T))

void ast_init(void* ptr,ast_dtor dtor,print_ast_element print,graph_ast_element graph,eval_ast_element eval)
{
  ast_t* this = ptr;
  this->dtor = dtor;
  this->print = print;
  this->graph = graph;
  this->eval = eval;
}

unary_op_t* new_unary_op(char o,void* l)
{
  unary_op_t* uo = NEW(unary_op_t);
  ast_init(uo,unary_op_dtor,unary_op_print,unary_op_graph,unary_op_eval);
  uo->op = o;
  uo->left=l;
  return uo;
}

void unary_op_dtor(void* uo)
{
  const unary_op_t* this = uo;
  delete_ast(this->left);
//  this->left->dtor(this->left);
}

void unary_op_print(const void* uo,FILE* o)
{
  const unary_op_t* this = uo;
  fprintf(o,"uop: %c\n",this->op);
  this->print(this,o);
}

void unary_op_graph(const void* uo,FILE* o)
{
  const unary_op_t* this = uo;
  this->left->graph(this->left,o);
  fprintf(o,"L%p [label= \"%c\"];\n",this,this->op);
  fprintf(o,"L%p -> L%p;\n",this,this->left);
}

double unary_op_eval(const void* uo)
{
  const unary_op_t* this = uo;
  double value = this->left->eval(this->left);
  if(this->op == '-')
    value = -value;
  return value;
}

binary_op_t* new_binary_op(void* l,char op,void* r)
{
  binary_op_t* this = NEW(binary_op_t);
  ast_init(this,binary_op_dtor,binary_op_print,binary_op_graph,binary_op_eval);
  this->left = l;
  this->op = op;
  this->right = r;
  return this;
}

void binary_op_dtor(void* bo)
{
  binary_op_t* this = bo;
  delete_ast(this->left);
  delete_ast(this->right);
//  this->left->dtor(this->left);
//  this->right->dtor(this->right);
}

void binary_op_print(const void* bo,FILE* o)
{
  const binary_op_t*this = bo;
  this->left->print(this->left,o);
  fprintf(o,"bop: %c\n",this->op);
  this->right->print(this->right,o);
}

double binary_op_eval(const void* bo)
{
  const binary_op_t* this = bo;
  double lv = this->left->eval(this->left);
  double rv = this->right->eval(this->right);
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

void binary_op_graph(const void* bo,FILE* o)
{
  const binary_op_t* this = bo;
  this->left->graph(this->left,o);
  this->right->graph(this->right,o);
  fprintf(o,"L%p [label= \"%c\"];\n",this,this->op);
  fprintf(o,"L%p -> L%p;\n",this,this->left);
  fprintf(o,"L%p -> L%p;\n",this,this->right);
}

number_t* new_number(double v)
{
  number_t* this = NEW(number_t);
  ast_init(this,number_dtor,number_print,number_graph,number_eval);
  this->value = v;
  return this;
}

void number_print(const void* n,FILE* o)
{
  const number_t* this = n;
  fprintf(o,"%lg\n",this->value);
}

void number_graph(const void* n,FILE* o)
{
  const number_t* this = n;
  fprintf(o,"L%p [label= \"%lg\"];\n",this,this->value);
}

double number_eval(const void* n)
{
  return ((const number_t*)n)->value;
}

variable_t* new_variable(const variable_description_t* var)
{
  variable_t* this= NEW(variable_t);
  ast_init(this,variable_dtor,variable_print,variable_graph,variable_eval);
  this->var = var;
  return this;
}

void variable_print(const void* v,FILE* o)
{
  const variable_t* this = v;
  fprintf(o,"%s\n",this->var->name);
}

void variable_graph(const void* v,FILE* o)
{
  const variable_t* this = v;
  fprintf(o,"L%p [label= \"%s\"];\n",this,this->var->name);
}

double variable_eval(const void* v)
{
  return *((const variable_t*)v)->var->pvalue;
}

math_function_t* new_math_function(const math_function_description_t* func,void* l)
{
  math_function_t* this = NEW(math_function_t);
  ast_init(this,math_function_dtor,math_function_print,math_function_graph,math_function_eval);
  this->func=func;
  this->left=l;
  return this;
}

void math_function_print(const void *mf,FILE* o)
{
  const math_function_t* this = mf;
  fprintf(o,"%s\n",this->func->name);
  this->left->print(this->left,o);
}

void math_function_graph(const void*mf,FILE* o)
{
  const math_function_t* this = mf;
  this->left->graph(this->left,o);
  fprintf(o,"L%p [label=\"%s\"];\n",this,this->func->name);
  fprintf(o,"L%p -> L%p;\n",this,this->left);
}

double math_function_eval(const void*mf)
{
  const math_function_t* this = mf;
  return this->func->func(this->left->eval(this->left));
}
