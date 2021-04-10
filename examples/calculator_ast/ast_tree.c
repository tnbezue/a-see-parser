#include <stdlib.h>
#include <math.h>
#include "ast_tree.h"


//void dtor_noop(void* ptr) { } // for objects that do not require a destructor

//void ast_init(void* ptr,ast_dtor dtor,print_ast_element print,graph_ast_element graph,eval_ast_element eval)

ast_class_table_t ast_class_table = {
  .parent_class_table_=&object_class_table,
  .class_name_="ast",
  .type_size_=sizeof(ast_t),
  .traits_=INSTANCE_HAS_CLASS_TABLE|TRIVALLY_DESTRUCTIBLE,
  .default_ctor = ast_default_ctor,
  .dtor = ast_dtor,
  .copy_ctor = ast_copy_ctor,
  .assign = ast_assign,
  .compare = ast_compare,
  .isa = object_isa,
  .print = NULL,
  .graph = NULL,
  .eval = NULL
};

void unary_op_ast_default_ctor(void* ptr)
{
  ((unary_op_ast_t*)ptr)->left=NULL;
}

void unary_op_ast_ctor(void* ptr,int op,void* l)
{
  unary_op_ast_t* this = ptr;
  this->op=op;
  this->left=l;
}

void unary_op_ast_dtor(void* uo)
{
  const unary_op_ast_t* this = uo;
  DELETE(this->left);
}

void unary_op_ast_assign(void* d,const void* s)
{
  unary_op_ast_t* dst = d;
  const unary_op_ast_t* src=s;
  dst->op = src->op;
  dst->left = src->left;
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

unary_op_ast_class_table_t unary_op_ast_class_table = {
  .parent_class_table_=(object_class_table_t*)&ast_class_table,
  .class_name_="unary_op_ast",
  .type_size_=sizeof(unary_op_ast_t),
  .traits_=INSTANCE_HAS_CLASS_TABLE,
  .default_ctor = unary_op_ast_default_ctor,
  .dtor = unary_op_ast_dtor,
  .copy_ctor = unary_op_ast_copy_ctor,
  .assign = unary_op_ast_assign,
  .compare = unary_op_ast_compare,
  .isa = object_isa,
  .print = unary_op_ast_print,
  .graph = unary_op_ast_graph,
  .eval = unary_op_ast_eval
};

void binary_op_ast_default_ctor(void* ptr)
{
  unary_op_ast_default_ctor(ptr);
  ((binary_op_ast_t*)ptr)->right = NULL;
}

void binary_op_ast_ctor(void*ptr,void* l,char op,void* r)
{
  binary_op_ast_t* this = ptr;
  unary_op_ast_ctor(this,op,l);
  this->right = r;
}

void binary_op_ast_dtor(void* bo)
{
  binary_op_ast_t* this = bo;
  DELETE(this->right);
}

void binary_op_ast_assign(void *d,const void* s)
{
  unary_op_ast_assign(d,s);
  ((binary_op_ast_t*)d)->right = ((const binary_op_ast_t*)s)->right;
}

void binary_op_ast_print(const void* bo,FILE* o)
{
  const binary_op_ast_t*this = bo;
  M(this->left,print,o);
  fprintf(o,"bop: %c\n",this->op);
  M(this->right,print,o);
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

binary_op_ast_class_table_t binary_op_ast_class_table = {
  .parent_class_table_=(object_class_table_t*)&unary_op_ast_class_table,
  .class_name_="binary_op_ast",
  .type_size_=sizeof(binary_op_ast_t),
  .traits_=INSTANCE_HAS_CLASS_TABLE,
  .default_ctor = binary_op_ast_default_ctor,
  .dtor = binary_op_ast_dtor,
  .copy_ctor = binary_op_ast_copy_ctor,
  .assign = binary_op_ast_assign,
  .compare = binary_op_ast_compare,
  .isa = object_isa,
  .print = binary_op_ast_print,
  .graph = binary_op_ast_graph,
  .eval = binary_op_ast_eval
};

void number_ast_ctor(void*ptr,double v)
{
  number_ast_t* this = ptr;
  this->value = v;
}

void number_ast_assign(void* d,const void* s)
{
  ((number_ast_t*)d)->value = ((const number_ast_t*)s)->value;
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

number_ast_class_table_t number_ast_class_table = {
  .parent_class_table_=(object_class_table_t*)&ast_class_table,
  .class_name_="number_ast",
  .type_size_=sizeof(number_ast_t),
  .traits_=INSTANCE_HAS_CLASS_TABLE|TRIVALLY_DESTRUCTIBLE,
  .default_ctor = number_ast_default_ctor,
  .dtor = number_ast_dtor,
  .copy_ctor = number_ast_copy_ctor,
  .assign = number_ast_assign,
  .compare = number_ast_compare,
  .isa = object_isa,
  .print = number_ast_print,
  .graph = number_ast_graph,
  .eval = number_ast_eval
};


void variable_ast_ctor(void* ptr,const variable_description_t* var)
{
  variable_ast_t* this= ptr;
  this->var = var;
}

void variable_ast_assign(void* d,const void* s)
{
  ((variable_ast_t*)d)->var = ((const variable_ast_t*)s)->var;
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

variable_ast_class_table_t variable_ast_class_table = {
  .parent_class_table_=(object_class_table_t*)&ast_class_table,
  .class_name_="variable_ast",
  .type_size_=sizeof(variable_ast_t),
  .traits_=INSTANCE_HAS_CLASS_TABLE|TRIVALLY_DESTRUCTIBLE,
  .default_ctor = variable_ast_default_ctor,
  .dtor = variable_ast_dtor,
  .copy_ctor = variable_ast_copy_ctor,
  .assign = variable_ast_assign,
  .compare = variable_ast_compare,
  .isa = object_isa,
  .print = variable_ast_print,
  .graph = variable_ast_graph,
  .eval = variable_ast_eval
};

void math_function_ast_default_ctor(void* ptr)
{
  unary_op_ast_default_ctor(ptr);
  ((math_function_ast_t*)ptr)->func = NULL;
}

void math_function_ast_assign(void* d,const void* s)
{
  unary_op_ast_assign(d,s);
  ((math_function_ast_t*)d)->func = ((const math_function_ast_t*)s)->func;
}

void math_function_ast_ctor(void* ptr,const math_function_description_t* func,void* l)
{
  math_function_ast_t* this = ptr;
  this->func=func;
  this->left=l;
}

void math_function_ast_print(const void *mf,FILE* o)
{
  const math_function_ast_t* this = mf;
  fprintf(o,"%s\n",this->func->name);
  M(this->left,print,o);
}

void math_function_ast_graph(const void*mf,FILE* o)
{
  const math_function_ast_t* this = mf;
  M(this->left,graph,o);
  fprintf(o,"L%p [label=\"%s\"];\n",this,this->func->name);
  fprintf(o,"L%p -> L%p;\n",this,this->left);
}

double math_function_ast_eval(const void*mf)
{
  const math_function_ast_t* this = mf;
  return this->func->func(M(this->left,eval));
}

math_function_ast_class_table_t math_function_ast_class_table = {
  .parent_class_table_=(object_class_table_t*)&unary_op_ast_class_table,
  .class_name_="math_function_ast",
  .type_size_=sizeof(math_function_ast_t),
  .traits_=INSTANCE_HAS_CLASS_TABLE,
  .default_ctor = math_function_ast_default_ctor,
  .dtor = math_function_ast_dtor,
  .copy_ctor = math_function_ast_copy_ctor,
  .assign = math_function_ast_assign,
  .compare = math_function_ast_compare,
  .isa = object_isa,
  .print = math_function_ast_print,
  .graph = math_function_ast_graph,
  .eval = math_function_ast_eval
};
