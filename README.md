# a-see-parser
A simple, easy, elegant (or silly, elementary, experimental) general purpose parser.
Allows a PEG-like grammar to be specified in directly in C.


## Example

PEG grammar for simple calculater

    expr <- term

    term <- factor ( ( '+' / '-' ) factor )*

    factor <- ( ('+' / '-' ) factor ) / ( power ( ( '*' / '/' ) power)* )

    power <- value ( '^' power )*

    value <- number / '(' term ')' / function / variable

    function <- ident '(' term ')'

    variable <- ident

Equivalent using a-see-parser

    int expr(a_see_parser_t* acp)
    {
      return term(acp) && NEXT_CHR(acp) == 0;
    }

    int term(a_see_parser_t* acp)
    {
      int c;
      return factor(acp) &&
        ZERO_OR_MORE(acp,((c = NEXT_CHR(acp)) == '+' || c == '-') && factor(acp),,);
    }

    int factor(a_see_parser_t* acp)
    {
      int c;
      return RULE(acp,((c = NEXT_CHR(acp)) == '+'  || c == '-') && factor(acp),,)
      ||
      (power(acp) &&
            ZERO_OR_MORE(acp,((c = NEXT_CHR(acp)) == '*' || c == '/') && power(acp),,));
    }

    int power(a_see_parser_t* acp)
    {
      return value(acp) && ZERO_OR_MORE(acp,NEXT_CHR(acp) == '^' && power(acp),,);
    }

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

    int number(a_see_parser_t* acp)
    {
      return RULE(acp,REAL(acp) || INTEGER(acp),,);
    }

    int function(a_see_parser_t* acp)
    {
      return RULE(acp,IDENT(acp) && NEXT_CHR(acp) == '('
              && term(acp) && NEXT_CHR(acp) == ')',,);
    }

    int variable(a_see_parser_t* acp)
    {
      return RULE(acp,IDENT(acp),,);
    }
