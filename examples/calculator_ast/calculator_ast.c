

/*
  Implements the following calculator grammar

  expr <- term

  term <- factor ( ( '+' / '-' ) factor )*

  factor <- ( ('+' / '-' ) factor ) / ( power ( ( '*' / '/' ) power)* )

  power <- value ( '^' power )*

  value <- number / '(' term ')' / function / variable

  function <- ident '(' term ')'

  variable <- ident

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <a_see_parser/a_see_parser.h>
#include "ast_tree.h"

DECLARE_DEFAULT_A_SEE_PARSER;
IMPLEMENT_DEFAULT_A_SEE_PARSER;

static math_function_description_t functions[] = {
  { "abs", fabs },
  { "acos", acos },
  { "asin", asin},
  { "atan", atan},
  { "ceil", ceil},
  { "cos", cos},
  { "cosh", cosh},
  { "exp", exp},
  { "floor", floor},
  { "log", log10},
  { "ln", log},
  { "sin", sin},
  { "sinh", sinh},
  { "sqrt", sqrt},
  { "tan", tan},
  { "tanh", tanh},
  { NULL, NULL }
};

/*
  returns address of function. NULL if not found
*/
const math_function_description_t* get_math_function(const char* name)
{
  const math_function_description_t* i = functions;
  for(;i->name != NULL;i++)
    if(strcmp(name,i->name) == 0)
      return i;
  return NULL;
}

static double x=99;
static double pi=M_PI;
static double e=M_E;
variable_description_t variables[] = {
  { "x", &x},
  { "pi", &pi},
  { "e", &e},
  { NULL,NULL}
};

const variable_description_t* get_variable(const char* name)
{
  const variable_description_t* i = variables;
  for(;i->name != NULL;i++)
    if(strcmp(name,i->name) == 0)
      return i;
  return NULL;
}

#define PLUS ((PEEK_CHR == '+' && NEXT_CHR) && SPACE)
#define MINUS ((PEEK_CHR == '-' && NEXT_CHR) && SPACE)
#define TIMES ((PEEK_CHR == '*' && NEXT_CHR) && SPACE)
#define DIVIDE ((PEEK_CHR == '/' && NEXT_CHR) && SPACE)
#define POWER ((PEEK_CHR == '^' && NEXT_CHR) && SPACE)
#define LPAREN ((PEEK_CHR == '(' && NEXT_CHR) && SPACE)
#define RPAREN ((PEEK_CHR == ')' && NEXT_CHR) && SPACE)

int expr(ast_t**);
int term(ast_t**);
int factor(ast_t**);
int power(ast_t**);
int value(ast_t**);
int function(ast_t**);
int number(ast_t**);
int variable(ast_t**);
// expr = term !.
int expr(ast_t** ast)
{
  return term(ast) && !ANY;
}

//  term <- factor ( ( '+' / '-' ) factor )*
int term(ast_t** last)
{
  int c;
  ast_t* rast=NULL;
  return factor(last) &&
    ZERO_OR_MORE(((PLUS && (c='+')) || (MINUS && (c='-'))) && factor(&rast),
      *last = (ast_t*)new_binary_op(*last,c,rast);
    ,);
}

//  factor <- ( ('+' / '-' ) factor ) / ( power ( '*' / '/' ) power )* )
int factor(ast_t** last)
{
  int c;
  ast_t* rast=NULL;
  return RULE(((PLUS && (c='+')) || (MINUS && (c='-'))) && factor(last),
      *last = (ast_t*)new_unary_op(c,*last);
    ,)
  ||
  (power(last) &&
        ZERO_OR_MORE(((TIMES && (c='*')) || (DIVIDE && (c='/'))) && power(&rast),
          *last = (ast_t*)new_binary_op(*last,c,rast);
        ,));
}

//  power <- value ( '^' power )*
int power(ast_t** last)
{
  ast_t* rast=NULL;
  return value(last) && ZERO_OR_MORE(POWER && power(&rast),
    *last = (ast_t*) new_binary_op(*last,'^',rast);
  ,);
}

//  value <- number / '(' term ')' / function / variable
int value(ast_t** ast)
{
  int rc=0;
  return number(ast)
    ||
    RULE(LPAREN && term(ast) && (rc = 1) && RPAREN,
    ,
      if(rc && *ast) delete_ast(*ast);
    )
    ||
    function(ast)
    ||
    variable(ast);
}

#define INTEGER (DECIMAL_INTEGER || HEX_INTEGER || OCTAL_INTEGER)
// number <- REAL / INTEGER
int number(ast_t** ast)
{
  char nstr[128];
  int rc=0;
  return RULE((REAL && (rc = 1)) || (INTEGER && (rc=2)),
    CAPTURE_TEXT(nstr,128);
    double d;
    if(rc==1)
      d=atof(nstr);
    else
      d=strtol(nstr,NULL,0);
    *ast = (ast_t*) new_number(d);
  ,);
}

//  function <- ident '(' term ')'
int function(ast_t** ast)
{
  char id[128];
  int rc=0;
  ast_t* fterm = NULL;
  return RULE(IDENTIFIER && CAPTURE_TEXT(id,128) && LPAREN
          && term(&fterm) && (rc = 1) && RPAREN,
          const math_function_description_t* func= get_math_function(id);
          if(func)
            *ast = (ast_t*) new_math_function(func,fterm);
          else {
            printf("Function %s is undefined\n",id);
            exit(1);
          }
          ,
            if(rc && fterm) delete_ast(fterm);
          );
}

int variable(ast_t** ast)
{
  return RULE(IDENTIFIER,
    char id[126];
    CAPTURE_TEXT(id,128);
    const variable_description_t *var = get_variable(id);
    *ast = (ast_t*)new_variable(var);
  ,);
}

int main(int argc,char* argv[])
{
  int i;
  for(i=1;i<argc;i++) {
    ast_t* ast=NULL;
    SET_PARSE_STRING(argv[i]);
    if(expr(&ast)) {
      x = 12.5;
      printf("(x=%lg) %s = %lg\n",x,argv[i],ast->vtable->eval(ast));
      x = -2.75;
      printf("(x=%lg) %s = %lg\n",x,argv[i],ast->vtable->eval(ast));
      x = 0.03;
      printf("(x=%lg) %s = %lg\n",x,argv[i],ast->vtable->eval(ast));
      char fname[32] = "Prob";
      sprintf(fname+4,"%d.gv",i);
      FILE* f = fopen(fname,"w");
      fprintf(f,"digraph \"%s\" {\n",argv[i]);
      ast->vtable->graph(ast,f);
      fprintf(f,"}\n");
      fclose(f);
      delete_ast(ast);
    }
    else
      printf("%s -- Failed\n",argv[i]);
   }
}
