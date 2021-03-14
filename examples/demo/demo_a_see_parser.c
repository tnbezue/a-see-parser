

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

int expr(a_see_parser_t*);
int term(a_see_parser_t*);
int factor(a_see_parser_t*);
int power(a_see_parser_t*);
int value(a_see_parser_t*);
int function(a_see_parser_t*);
int number(a_see_parser_t*);
int variable(a_see_parser_t*);
// expr = term
// Success if all tokens consumed
int expr(a_see_parser_t* acp)
{
  return term(acp) && NEXT_CHR(acp) == 0;
}

//  term = factor ( ( '+' | '-' ) factor )*
int term(a_see_parser_t* acp)
{
  int c;
  return factor(acp) &&
    ZERO_OR_MORE(acp,((c = NEXT_CHR(acp)) == '+' || c == '-') && factor(acp),,);
}

//  factor = ( ('+' | '-' ) factor ) | ( power ( ( '*' | '/' ) power)* )
int factor(a_see_parser_t* acp)
{
  int c;
  return RULE(acp,((c = NEXT_CHR(acp)) == '+'  || c == '-') && factor(acp),,)
  ||
  (power(acp) &&
        ZERO_OR_MORE(acp,((c = NEXT_CHR(acp)) == '*' || c == '/') && power(acp),,));
}

//  power = value ( '^' power )*
int power(a_see_parser_t* acp)
{
  return value(acp) && ZERO_OR_MORE(acp,NEXT_CHR(acp) == '^' && power(acp),,);
}

//  value = number | '(' term ')' | function | variable
int value(a_see_parser_t* acp)
{
  return number(acp)
    ||
    RULE(acp,NEXT_CHR(acp) == '(' && term(acp) && NEXT_CHR(acp) == ')',,)
    ||
    function(acp)
    ||
    variable(acp);
}
#define REAL(ACP) a_see_parser_floating_point(ACP)
#define INTEGER(ACP) (a_see_parser_decimal_integer(ACP) || a_see_parser_octal_integer(ACP) \
    || a_see_parser_hex_integer(ACP))
// number <- REAL / INTEGER
int number(a_see_parser_t* acp)
{
  return RULE(acp,REAL(acp) || INTEGER(acp),,);
}

#define IDENT(ACP) a_see_parser_ident(ACP)
//  function = ident '(' term ')'
int function(a_see_parser_t* acp)
{
  return RULE(acp,IDENT(acp) && NEXT_CHR(acp) == '('
          && term(acp) && NEXT_CHR(acp) == ')',,);
}

int variable(a_see_parser_t* acp)
{
  return RULE(acp,IDENT(acp),,);
}

int main(int argc,char* argv[])
{
  int i;
  a_see_parser_t *acp = new_a_see_parser();
  for(i=1;i<argc;i++) {
    a_see_parser_set_string(acp,argv[i]);
    printf("%s -- %s\n",argv[i],expr(acp) ? "Success" : "Fail");
    a_see_parser_reset(acp);
  }
  delete_a_see_parser(acp);
}
