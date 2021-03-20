/*
		Copyright (C) 2021  by Terry N Bezue

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <a_see_parser/a_see_parser.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

int a_see_parser_next_chr(a_see_parser_t* acp)
{
  int c = A_SEE_PARSER_PEEK_CHR(acp);
  if(c) {
    acp->ptr_++;
    if(c=='\n')
      acp->line_number_++;
  }
  return c;
}

static const char* whitespace_chars = " \t";
static const char* whitespace_chars_with_eol = " \t\r\n";

int a_see_parser_whitespace(a_see_parser_t* acp,int include_new_line_as_whitespace)
{
  const char* ws_chars = include_new_line_as_whitespace ? whitespace_chars_with_eol : whitespace_chars;
  int c,nchar=0;
  while((c=A_SEE_PARSER_PEEK_CHR(acp))) {
    if(strchr(ws_chars,c)==NULL)
      break;
    nchar++;
    a_see_parser_next_chr(acp);
  }
  return nchar;
}

int a_see_parser_eol(a_see_parser_t* acp)
{
  int rc = 0;
  int c=A_SEE_PARSER_PEEK_CHR(acp);
  if(c=='\r') {
    // Mac or windows
    rc = 1;
    A_SEE_PARSER_NEXT_CHR(acp);
    c=A_SEE_PARSER_PEEK_CHR(acp);
    if(c == '\n') // WINDOWS
       A_SEE_PARSER_NEXT_CHR(acp);
  } else if(c=='\n') { // Linux or Unix
    A_SEE_PARSER_NEXT_CHR(acp);
    rc = 1;
  }
  return rc;
/*  return A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_NEXT_CHR(acp) == '\r' && A_SEE_PARSER_NEXT_CHR(acp) == '\n')
    ||
    A_SEE_PARSER_SIMPLE_RULE(acp,(c=A_SEE_PARSER_NEXT_CHR(acp)) == '\n' || c == '\r');*/
}

int a_see_parser_char_sequence(a_see_parser_t*acp,const char*str)
{
  return A_SEE_PARSER_SIMPLE_RULE(acp,
    ({
      int rc=1;
      int c;
      while(*str && (c=A_SEE_PARSER_NEXT_CHR(acp))) {
        if(c != *str) {
          rc = 0;
          break;
        }
        str++;
      }
      rc;
    })
  );
}

int a_see_parser_delimited_comment(a_see_parser_t*acp,const char* start,const char* end,int nested)
{
  int len=0;
  const char* ptr = acp->ptr_;
  A_SEE_PARSER_SIMPLE_RULE(acp,
      A_SEE_PARSER_CHAR_SEQUENCE(acp,start) && A_SEE_PARSER_ZERO_OR_MORE(acp,!A_SEE_PARSER_CHAR_SEQUENCE(acp,end) &&
        ((nested && a_see_parser_delimited_comment(acp,start,end,nested)) || 1) &&
        A_SEE_PARSER_NEXT_CHR(acp),,) && A_SEE_PARSER_CHAR_SEQUENCE(acp,end)
  );
  len = acp->ptr_-ptr;
  return len;
}

int a_see_parser_one_line_comment(a_see_parser_t*acp,const char* start)
{
  int len=0;
  const char* ptr = acp->ptr_;
  A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_CHAR_SEQUENCE(acp,start) && A_SEE_PARSER_ZERO_OR_MORE(acp,!A_SEE_PARSER_END_OF_LINE(acp) && A_SEE_PARSER_NEXT_CHR(acp),,)
    && A_SEE_PARSER_END_OF_LINE(acp)
  );
  len=acp->ptr_-ptr;
  return len;
}

int a_see_parser_space(a_see_parser_t* acp)
{
  A_SEE_PARSER_ZERO_OR_MORE(acp,a_see_parser_whitespace(acp,acp->flags_&INCLUDE_EOL_IN_WHITESPACE)
    || a_see_parser_c_comment(acp) || a_see_parser_cpp_comment(acp),,);
  return 1;
}

int a_see_parser_capture_length(a_see_parser_t* acp)
{
  int len = 0;
  if(acp->capture_.end_ && acp->capture_.begin_ && acp->capture_.end_>acp->capture_.begin_)
    len = acp->capture_.end_-acp->capture_.begin_;
  return len;
}

int a_see_parser_capture_text(a_see_parser_t* acp,char* str,unsigned int len)
{
  if(str && len) {
    unsigned int length =A_SEE_PARSER_CAPTURE_LENGTH(acp);
    if(length) {
      if(len < length)
        length = len;
      strncpy(str,acp->capture_.begin_,length);
    }
    str[length]=0;
  }
  return 1;
}

// Works like [-a-zA-Z0-9]
int a_see_parser_range(a_see_parser_t* acp,const char* range)
{
  int rc=0;
  int c = A_SEE_PARSER_PEEK_CHR(acp);
  int fc;
  int not = 0;
  if(*range == '^') {
    not = 1;
    range++;
  }
  for(;*range;range++) {
    fc=*range;
    if(*(range+1)== '-') {
      range+=2;
      if(*range) {
        if(fc <= c && c <= *range) {
          rc = 1;
          break;
        }
      } else
        break;
    } else {
      if(c == fc) {
        rc=c;
        break;
      }
    }
  }
  if(not)
    rc = !rc;
  if(rc)
    A_SEE_PARSER_NEXT_CHR(acp);
  return rc;
}

#define A_SEE_PARSER_EXPONENT  A_SEE_PARSER_RANGE(acp,"eE") && \
  A_SEE_PARSER_OPTIONAL(acp,A_SEE_PARSER_RANGE(acp,"-+"),,) && \
  A_SEE_PARSER_ONE_OR_MORE(acp,A_SEE_PARSER_RANGE(acp,"0-9"),,)

// [0-9]+ '.' [0-9]*
#define A_SEE_PARSER_FLOAT1 A_SEE_PARSER_ONE_OR_MORE(acp, \
        A_SEE_PARSER_RANGE(acp,"0-9"),,) && A_SEE_PARSER_NEXT_CHR(acp)=='.' && \
        A_SEE_PARSER_ZERO_OR_MORE(acp,A_SEE_PARSER_RANGE(acp,"0-9"),,)

// [0-9]+ ('.' [0-9]*)?
#define A_SEE_PARSER_FLOAT2 A_SEE_PARSER_ONE_OR_MORE(acp, \
        A_SEE_PARSER_RANGE(acp,"0-9"),,) && A_SEE_PARSER_OPTIONAL(acp, \
        A_SEE_PARSER_NEXT_CHR(acp)=='.' && \
        A_SEE_PARSER_ZERO_OR_MORE(acp,A_SEE_PARSER_RANGE(acp,"0-9"),,),,)

// [0-9]* '.' [0-9]+ ([eE] [0-9]+)?
#define A_SEE_PARSER_FLOAT3 A_SEE_PARSER_ZERO_OR_MORE(acp, \
        A_SEE_PARSER_RANGE(acp,"0-9"),,) && A_SEE_PARSER_NEXT_CHR(acp)=='.' && \
        A_SEE_PARSER_ONE_OR_MORE(acp,A_SEE_PARSER_RANGE(acp,"0-9"),,)

#define __FLOATING_POINT_OPTION_A__
int a_see_parser_floating_point(a_see_parser_t*acp)
{
#ifdef __FLOATING_POINT_OPTION_A__
  return A_SEE_PARSER_CAPTURE_ON(acp) &&
    (
        A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_FLOAT1 &&
            A_SEE_PARSER_OPTIONAL(acp,A_SEE_PARSER_EXPONENT,,))
      ||
      A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_FLOAT2 &&
          A_SEE_PARSER_EXPONENT)
      ||
      A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_FLOAT3 &&
          A_SEE_PARSER_OPTIONAL(acp,A_SEE_PARSER_EXPONENT,,))
    ) && A_SEE_PARSER_CAPTURE_OFF(acp) && A_SEE_PARSER_SPACE(acp);

#else
  int rc=0;
  return A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_CAPTURE_ON(acp) &&
      ({
        int c;
        int has_whole = 0;
        int has_decimal_point = 0;
        int ndecimal_digits=0;
        if(isdigit(A_SEE_PARSER_PEEK_CHR(acp))) {
          A_SEE_PARSER_NEXT_CHR(acp);
          for(;isdigit(A_SEE_PARSER_PEEK_CHR(acp));A_SEE_PARSER_NEXT_CHR(acp));
          has_whole = 1;
        }
        if (A_SEE_PARSER_PEEK_CHR(acp) == '.') {
          has_decimal_point = 1;
          A_SEE_PARSER_NEXT_CHR(acp);
          for(;isdigit(A_SEE_PARSER_PEEK_CHR(acp));A_SEE_PARSER_NEXT_CHR(acp)) ndecimal_digits++;
        }
        if(has_whole || ndecimal_digits ) {
          int has_exponent = 0;
          if((c=A_SEE_PARSER_PEEK_CHR(acp)) == 'e' || c == 'E') {
            A_SEE_PARSER_NEXT_CHR(acp);
            if((c=A_SEE_PARSER_PEEK_CHR(acp)) == '+' || c == '-') {
              A_SEE_PARSER_NEXT_CHR(acp);
            }
            int ndigits = 0;
            for(;isdigit(A_SEE_PARSER_PEEK_CHR(acp));A_SEE_PARSER_NEXT_CHR(acp)) ndigits++;
            if(ndigits > 0)
              has_exponent = 1;
          }
          if((has_whole && has_decimal_point) || (has_whole && has_exponent)
              || (ndecimal_digits))
            rc=1;
        }
        rc;
      }) && A_SEE_PARSER_CAPTURE_OFF(acp) && A_SEE_PARSER_SPACE(acp));
#endif
}

int a_see_parser_decimal_integer(a_see_parser_t*acp)
{
  return A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_CAPTURE_ON(acp) &&
      A_SEE_PARSER_RANGE(acp,"1-9") &&  A_SEE_PARSER_ZERO_OR_MORE(acp,A_SEE_PARSER_RANGE(acp,"0-9"),,)) && A_SEE_PARSER_CAPTURE_OFF(acp) && a_see_parser_space(acp);
}

int a_see_parser_octal_integer(a_see_parser_t*acp)
{
  return A_SEE_PARSER_CAPTURE_ON(acp) && A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_NEXT_CHR(acp)=='0' &&
      A_SEE_PARSER_ZERO_OR_MORE(acp,A_SEE_PARSER_RANGE(acp,"0-9"),,)) && A_SEE_PARSER_CAPTURE_OFF(acp) && a_see_parser_space(acp);
}

int a_see_parser_hex_integer(a_see_parser_t*acp)
{
  int c;
  return A_SEE_PARSER_CAPTURE_ON(acp) && A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_NEXT_CHR(acp)=='0' && ((c=A_SEE_PARSER_NEXT_CHR(acp)) == 'x' || c=='X')
        && A_SEE_PARSER_ONE_OR_MORE(acp,A_SEE_PARSER_RANGE(acp,"0-9a-fA-F"),,)) && A_SEE_PARSER_CAPTURE_OFF(acp) && a_see_parser_space(acp);
}

int a_see_parser_ident(a_see_parser_t* acp)
{
  return A_SEE_PARSER_CAPTURE_ON(acp) && A_SEE_PARSER_SIMPLE_RULE(acp,
         A_SEE_PARSER_RANGE(acp,"_a-zA-Z") &&
        A_SEE_PARSER_ZERO_OR_MORE(acp,A_SEE_PARSER_RANGE(acp,"_a-zA-Z0-9"),,))
        && A_SEE_PARSER_CAPTURE_OFF(acp) && a_see_parser_space(acp);
}

/*
  Allow for escaped characters.
*/
#define __QUOTED_STRING_OPTION_A__
int a_see_parser_quoted_string(a_see_parser_t* acp,int ch)
{
#ifdef __QUOTED_STRING_OPTION_A__
  int c;
  return A_SEE_PARSER_CAPTURE_ON(acp) && A_SEE_PARSER_SIMPLE_RULE(acp,A_SEE_PARSER_NEXT_CHR(acp) == ch &&
        A_SEE_PARSER_ZERO_OR_MORE(acp,(c=A_SEE_PARSER_NEXT_CHR(acp))!=ch && c!=0 && ((c=='\\' && A_SEE_PARSER_NEXT_CHR(acp))  || 1),,))
       && A_SEE_PARSER_NEXT_CHR(acp) == '"' && A_SEE_PARSER_CAPTURE_OFF(acp);
#else
  int rc=0;
  return A_SEE_PARSER_CAPTURE_ON(acp) && A_SEE_PARSER_SIMPLE_RULE(acp,
    ({
      if(A_SEE_PARSER_NEXT_CHR(acp) == ch) {
        int c;
        while((c=A_SEE_PARSER_NEXT_CHR(acp))) {
          if(c == ch) {
            rc=1;
            break;
          } else if(c == '\\') {
            A_SEE_PARSER_NEXT_CHR(acp);
          }
        }
      }
      rc;
    })
  ) && A_SEE_PARSER_CAPTURE_OFF(acp) && a_see_parser_space(acp);
#endif
}
