# a-see-parser
A simple, easy, elegant (or silly, elementary, experimental) general purpose parser.
Allows a PEG-like grammar to be specified in C.


## Example

PEG grammar for simple calculater

    expr <- term !.

    term <- factor ( ( '+' / '-' ) factor )*

    factor <- ( ('+' / '-' ) factor ) / ( power ( ( '*' / '/' ) power)* )

    power <- value ( '^' power )*

    value <- number / '(' term ')' / function / variable

    function <- ident '(' term ')'

    variable <- ident

Equivalent using a-see-parser

    int expr()
    {
      return term() && !ANY;
    }

    int term()
    {
      int c;
      return factor() &&
        ZERO_OR_MORE(((c=NEXT_CHR) == '+' || c == '-') && factor(),,);
    }

    int factor()
    {
      int c;
      return RULE(((c=NEXT_CHR) == '+' || c == '-') && factor(),,)
      ||
      (power() &&
            ZERO_OR_MORE(((c=NEXT_CHR) == '*' || c == '/') && power(),,));
    }

    int power()
    {
      return value() && ZERO_OR_MORE(NEXT_CHR == '^' && power(),,);
    }

    int value()
    {
      return number()
        ||
        RULE(NEXT_CHR == '(' && term() && NEXT_CHR == ')',,)
        ||
        function()
        ||
        variable();
    }

    int number()
    {
      return RULE(REAL || INTEGER,,);
    }

    int function()
    {
      return RULE(IDENTIFIER && NEXT_CHR == '('
              && term() && NEXT_CHR == ')',,);
    }

    int variable()
    {
      return RULE(IDENTIFIER,,);
    }
