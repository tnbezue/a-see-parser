

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


#define PLUS ((PEEK_CHR == '+' && NEXT_CHR) && SPACE)
#define MINUS ((PEEK_CHR == '-' && NEXT_CHR) && SPACE)
#define TIMES ((PEEK_CHR == '*' && NEXT_CHR) && SPACE)
#define DIVIDE ((PEEK_CHR == '/' && NEXT_CHR) && SPACE)
#define POWER ((PEEK_CHR == '^' && NEXT_CHR) && SPACE)
#define LPAREN ((PEEK_CHR == '(' && NEXT_CHR) && SPACE)
#define RPAREN ((PEEK_CHR == ')' && NEXT_CHR) && SPACE)


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
  return SPACE && term() && !ANY;
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

#define INTEGER (DECIMAL_INTEGER || HEX_INTEGER || OCTAL_INTEGER)
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
  a_see_parser_t calculator_parser = A_SEE_PARSER_DEFUALT;
  __global_a_see_parser_pointer__ = &calculator_parser;
  for(i=1;i<argc;i++) {
    __global_a_see_parser_pointer__->ptr_=argv[i];
    printf("%s -- %s\n",argv[i],expr() ? "Success" : "Fail");
  }
}
