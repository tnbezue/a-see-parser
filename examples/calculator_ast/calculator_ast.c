

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
#include <time.h>
#define __USE_A_SEE_PARSER_PREDICATES__
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

#define SPACING (ACP_WHITESPACE(" \t\r\n"),1)
#define PLUS (NEXT_CHR_IS('+') && SPACING)
#define MINUS (NEXT_CHR_IS('-') && SPACING)
#define TIMES (NEXT_CHR_IS('*') && SPACING)
#define DIVIDE (NEXT_CHR_IS('/') && SPACING)
#define POWER (NEXT_CHR_IS('^') && SPACING)
#define LPAREN (NEXT_CHR_IS('(') && SPACING)
#define RPAREN (NEXT_CHR_IS(')') && SPACING)
#define REAL (ACP_FLOATING_POINT && SPACING)
#define IDENTIFIER (ACP_IDENTIFIER && SPACING)
#define INTEGER ((ACP_DECIMAL_INTEGER || ACP_HEX_INTEGER || ACP_OCTAL_INTEGER) && SPACING)

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
  return SPACING && term(ast) && !ANY;
}

//  term <- factor ( ( '+' / '-' ) factor )*
int term(ast_t** last)
{
  int c;
  ast_t* rast=NULL;
  return factor(last) &&
    ZERO_OR_MORE(((PLUS && (c='+')) || (MINUS && (c='-'))) && factor(&rast),
      *last = (ast_t*)NEW_CTOR(binary_op_ast,binary_op_ast_ctor,*last,c,rast);
    ,);
}

//  factor <- ( ('+' / '-' ) factor ) / ( power ( '*' / '/' ) power )* )
int factor(ast_t** last)
{
  int c;
  ast_t* rast=NULL;
  return RULE(((PLUS && (c='+')) || (MINUS && (c='-'))) && factor(last),
      *last = (ast_t*)NEW_CTOR(unary_op_ast,unary_op_ast_ctor,c,*last);
    ,)
  ||
  (power(last) &&
        ZERO_OR_MORE(((TIMES && (c='*')) || (DIVIDE && (c='/'))) && power(&rast),
          *last = (ast_t*)NEW_CTOR(binary_op_ast,binary_op_ast_ctor,*last,c,rast);
        ,));
}

//  power <- value ( '^' power )*
int power(ast_t** last)
{
  ast_t* rast=NULL;
  return value(last) && ZERO_OR_MORE(POWER && power(&rast),
    *last = (ast_t*) NEW_CTOR(binary_op_ast,binary_op_ast_ctor,*last,'^',rast);
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
      if(rc && *ast) DELETE(*ast);
    )
    ||
    function(ast)
    ||
    variable(ast);
}

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
    *ast = (ast_t*) NEW_CTOR(number_ast,number_ast_ctor,d);
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
            *ast = (ast_t*) NEW_CTOR(math_function_ast,math_function_ast_ctor,func,fterm);
          else {
            printf("Function %s is undefined\n",id);
            exit(1);
          }
          ,
            if(rc && fterm) DELETE(fterm);
          );
}

int variable(ast_t** ast)
{
  return RULE(IDENTIFIER,
    char id[126];
    CAPTURE_TEXT(id,128);
    const variable_description_t *var = get_variable(id);
    *ast = (ast_t*)NEW_CTOR(variable_ast,variable_ast_ctor,var);
  ,);
}

#define fMin -10
#define fMax 10
double fRand()
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

int main(int argc,char* argv[])
{
  int i;
  srand(time(NULL));
  for(i=1;i<argc;i++) {
    ast_t* ast=NULL;
    SET_PARSE_STRING(argv[i]);
    if(expr(&ast)) {
      x = fRand();
      printf("(x=%lg) %s = %lg\n",x,argv[i],M(ast,eval));
      x = fRand();
      printf("(x=%lg) %s = %lg\n",x,argv[i],M(ast,eval));
      x = fRand();
      printf("(x=%lg) %s = %lg\n",x,argv[i],M(ast,eval));
      char fname[32] = "Prob";
      sprintf(fname+4,"%d.gv",i);
      FILE* f = fopen(fname,"w");
      fprintf(f,"digraph \"%s\" {\n",argv[i]);
      M(ast,graph,f);
      fprintf(f,"}\n");
      fclose(f);
      DELETE(ast);
    }
    else
      printf("%s -- Failed\n",argv[i]);
   }
}
