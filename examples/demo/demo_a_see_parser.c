

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

int expr();
int term();
int factor();
int power();
int value();
int function();
int number();
int variable();
// expr = term
// Success if all tokens consumed
int expr()
{
  return SPACING && term() && !ANY;
}

//  term = factor ( ( '+' | '-' ) factor )*
int term()
{
//  int c;
  return factor() &&
    ZERO_OR_MORE((PLUS || MINUS) && factor(),,);
}

//  factor = ( ('+' | '-' ) factor ) | ( power ( ( '*' | '/' ) power)* )
int factor()
{
  return RULE((PLUS || MINUS) && factor(),,)
  ||
  (power() &&
        ZERO_OR_MORE((TIMES || DIVIDE) && power(),,));
}

//  power = value ( '^' power )*
int power()
{
  return value() && ZERO_OR_MORE(POWER && power(),,);
}

//  value = number | '(' term ')' | function | variable
int value()
{
  return number()
    ||
    RULE(LPAREN && term() && RPAREN,,)
    ||
    function()
    ||
    variable();
}

// number <- REAL / INTEGER
int number()
{
  return RULE(REAL || INTEGER,,);
}

//  function = ident '(' term ')'
int function()
{
  return RULE(IDENTIFIER && LPAREN && term() && RPAREN,,);
}

int variable()
{
  return RULE(IDENTIFIER,,);
}

a_see_parser_t* __global_a_see_parser_pointer__;
int main(int argc,char* argv[])
{
  int i;
  for(i=1;i<argc;i++) {
    SET_PARSE_STRING(argv[i]);
    printf("%s -- %s\n",argv[i],expr() ? "Success" : "Fail");
  }
}
