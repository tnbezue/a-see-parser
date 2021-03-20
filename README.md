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
      return SPACE && term() && !ANY;
    }

    int term()
    {
      return factor() && ZERO_OR_MORE((PLUS || MINUS) && factor(),,);
    }

    int factor()
    {
      return RULE((PLUS || MINUS) && factor(),,)
      ||
      (power() && ZERO_OR_MORE((TIMES || DIVIDE) && power(),,));
    }

    int power()
    {
      return value() && ZERO_OR_MORE(POWER && power(),,);
    }

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

    int number()
    {
      return RULE(REAL || INTEGER,,);
    }

    int function()
    {
      return RULE(IDENTIFIER && LPAREN && term() && RPAREN,,);
    }

    int variable()
    {
      return RULE(IDENTIFIER,,);
    }
