

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
#define __USE_A_SEE_PARSER_PREDICATES__
#include <a_see_parser/a_see_parser.h>

DECLARE_DEFAULT_A_SEE_PARSER;
IMPLEMENT_DEFAULT_A_SEE_PARSER;

typedef double (*math_function)(double);
typedef struct {
  const char* name;
  math_function func;
} math_function_description_t;

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

typedef struct {
  const char* name;
  double* pvalue;
} variable_description_t;

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

int expr(double*);
int term(double*);
int factor(double*);
int power(double*);
int value(double*);
int function(double*);
int number(double*);
int variable(double*);

// expr <- term !.
int expr(double* val)
{
  return SPACING && term(val) && !ANY;
}

//  term <- factor ( ( '+' { c = '+'; } ? '-' { c='-'; } ) factor )*
int term(double* lval)
{
  int c;
  double rval;
  return factor(lval) &&
    ZERO_OR_MORE(((PLUS && (c='+')) || (MINUS && (c='-'))) && factor(&rval),
      if(c == '+')
        *lval += rval;
      else
        *lval -= rval;
    ,);
}

//  factor = ( ('+' | '-' ) factor ) | ( power ( ( '*' | '/' ) power)* )
int factor(double* lval)
{
  int c;
  double rval;
  return RULE(((PLUS && (c='+')) || (MINUS && (c='-'))) && factor(lval),
    if(c == '-')
      *lval = -*lval;
    ,)
  ||
  (power(lval) &&
        ZERO_OR_MORE(((TIMES && (c='*')) || (DIVIDE && (c='/'))) && power(&rval),
          if(c=='*')
            *lval *= rval;
          else
            *lval /= rval;
        ,));
}

//  power = value ( '^' power )*
int power(double* lval)
{
  double rval;
  return value(lval) && ZERO_OR_MORE(POWER && power(&rval),
    *lval = pow(*lval,rval);
  ,);
}

//  value = number | '(' term ')' | function | variable
int value(double* val)
{
  return number(val)
    ||
    RULE(LPAREN && term(val) && RPAREN,,)
    ||
    function(val)
    ||
    variable(val);
}

// number <- REAL / INTEGER
int number(double* val)
{
  int rc=0;
  char nstr[128];
  return RULE((REAL && (rc = 1))|| (INTEGER && (rc=2)),
    CAPTURE_TEXT(nstr,128);
    if(rc==1)
      *val=atof(nstr);
    else
      *val=strtol(nstr,NULL,0);
  ,);
}

//  function = ident '(' term ')'
int function(double* val)
{
  char id[128];
  return RULE(IDENTIFIER && CAPTURE_TEXT(id,128) && LPAREN
          && term(val) && RPAREN,
          const math_function_description_t* func= get_math_function(id);
          if(func)
            *val = func->func(*val);
          else {
            printf("Function %s is undefined\n",id);
            exit(1);
          }
          ,);
}

int variable(double* val)
{
  return RULE(IDENTIFIER,
    char id[126];
    CAPTURE_TEXT(id,128);
    const variable_description_t *var = get_variable(id);
    if(var)
      *val = *var->pvalue;
    else {
      printf("Variable %s is undefined\n",id);
      exit(1);
    }
  ,);
}

int main(int argc,char* argv[])
{
  int i;
  for(i=1;i<argc;i++) {
    double val;
    SET_PARSE_STRING(argv[i]);
    printf("%s ",argv[1]);
    if(expr(&val))
      printf(" = %lg\n",val);
    else
      printf(" -- Failed\n");
  }
}
