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

#ifndef __A_SEE_PARSER_INCLUDED__
#define __A_SEE_PARSER_INCLUDED__

#include <stdlib.h>
#include <a_see_parser/simple_array.h>

typedef struct {
  char* text_;
  unsigned int length_;
} dynamic_string_t;

void dynamic_string_ctor(dynamic_string_t* ds);
void dynamic_string_dtor(dynamic_string_t* ds);
#define new_dynamic_string() NEW(dynamic_string_t,dynamic_string_ctor)
#define delete_dynamic_string(ds) DELETE(ds,dynamic_string_dtor)

void dynamic_string_set(dynamic_string_t*,const char*,unsigned int);
typedef unsigned int a_see_parser_size_t;

typedef struct {
  const char* ptr_;
  a_see_parser_size_t line_number_;
} a_see_parser_position_t;

void a_see_parser_position_init(a_see_parser_position_t* acp);


typedef struct {
  const char* ptr_; // pointer to postion in parse string
  simple_array_t* stack_; // saves postion of parser
  const char* capture_begin_;
  const char* capture_end_;
  dynamic_string_t* text_; // accumulated text of last capture
  a_see_parser_size_t line_number_;
  unsigned int flags_;
} a_see_parser_t;

#define ALLOW_NESTED_COMMENTS 1
#define ALLOW_EOL_IN_STRING 2
#define INCLUDE_EOL_IN_WHITESPACE 4
#define CASE_SENSITIVE 8

#define A_SEE_PARSER_DEFAULT_FLAGS ALLOW_NESTED_COMMENTS | INCLUDE_EOL_IN_WHITESPACE

void a_see_parser_init(a_see_parser_t*);
void a_see_parser_dtor(a_see_parser_t*);
void a_see_parser_set_string(a_see_parser_t*,const char*);
void a_see_parser_reset(a_see_parser_t*);
#define new_a_see_parser() NEW(a_see_parser_t,a_see_parser_init)
#define delete_a_see_parser(acp) DELETE(acp,a_see_parser_dtor)
void a_see_parser_rule_start(a_see_parser_t*);
void a_see_parser_rule_success(a_see_parser_t*);
void a_see_parser_rule_fail(a_see_parser_t*);
int a_see_parser_peek_chr(a_see_parser_t*);
int a_see_parser_next_chr(a_see_parser_t*);
//int a_see_parser_current_chr(a_see_parser_t*);
int a_see_parser_eol(a_see_parser_t*);
int a_see_parser_range(a_see_parser_t* acp,const char* range);

int a_see_parser_delimited_comment(a_see_parser_t*,const char* start,const char* end,int nested);
#define a_see_parser_c_comment(acp) a_see_parser_delimited_comment(acp,"/*","*/",acp->flags_ & ALLOW_NESTED_COMMENTS)
#define a_see_parser_pascal_comment(acp) a_see_parser_delimited_comment(acp,"(*","*)",acp->flags_ & ALLOW_NESTED_COMMENTS)
int a_see_parser_one_line_comment(a_see_parser_t*,const char* start);
#define a_see_parser_cpp_comment(acp) a_see_parser_one_line_comment(acp,"//")
int a_see_parser_whitespace(a_see_parser_t*,int include_new_line_as_whitespace);
int a_see_parser_space(a_see_parser_t*);
int a_see_parser_char_sequence(a_see_parser_t*,const char* include);
int a_see_parser_floating_point(a_see_parser_t*);
int a_see_parser_decimal_integer(a_see_parser_t*);
int a_see_parser_octal_integer(a_see_parser_t*);
int a_see_parser_hex_integer(a_see_parser_t*);
int a_see_parser_ident(a_see_parser_t*);
int a_see_parser_capture(a_see_parser_t*,int);
const char* a_see_parser_capture_text(a_see_parser_t*);

#define NEXT_CHR(ACP) a_see_parser_next_chr(ACP)
#define PEEK_CHR(ACP) a_see_parser_peek_chr(ACP)

#define RULE(ACP,rule,action,cleanup) \
  ({ \
    a_see_parser_rule_start(ACP); \
    int __rC__ =0; \
    if(rule) { a_see_parser_rule_success(ACP); __rC__=1; { action }  } \
     else { { cleanup } a_see_parser_rule_fail(ACP); } \
    __rC__; \
  })

#define OPTIONAL(ACP,rule,action,cleanup) \
  ({ \
    a_see_parser_rule_start(ACP); \
    if(rule) { a_see_parser_rule_success(ACP); { action }  } else { { cleanup } a_see_parser_rule_fail(ACP); } \
    1; \
  })

#define ZERO_OR_MORE(ACP,rule,action,cleanup) \
  ({ \
    do { a_see_parser_rule_start(ACP); \
      if(rule) { a_see_parser_rule_success(ACP); { action } } \
      else { { cleanup} a_see_parser_rule_fail(ACP); break; } \
    } while(1); \
    1; \
  })

#define ONE_OR_MORE(ACP,rule,action,cleanup) \
  ({\
    int __cOuNt__=0;\
    do { a_see_parser_rule_start(ACP); \
      if(rule) { \
        __cOuNt__++; a_see_parser_rule_success(ACP); \
        { action } \
      } else { \
        a_see_parser_rule_fail(ACP); \
       { cleanup } break; \
      } \
    } while(1); \
    __cOuNt__>0; \
  })

#define SEQUENCE(ACP,seq) a_see_parser_char_sequence(ACP,seq)

#define END_OF_LINE(ACP) a_see_parser_eol(ACP)

#define END_OF_INPUT(ACP) SIMPLE_RULE(NEXT_CHR(ACP) == 0)
/*
  A simple rule is same as RULE except action and clearnup not required
*/
#define SIMPLE_RULE(ACP,rule) \
      (a_see_parser_rule_start(ACP),(rule) ? (a_see_parser_rule_success(ACP),1) : (a_see_parser_rule_fail(ACP),0))

#define NON_CONSUMING_RULE(ACP,rule) \
  ({ \
    a_see_parser_rule_start(ACP);\
    int rc=rule;\
    a_see_parser_rule_fail(ACP);\
    rc;\
  })

#define RANGE(ACP,str) a_see_parser_range(acp,str)
#define CAPTURE_ON(ACP) a_see_parser_capture(ACP,1)
#define CAPTURE_OFF(ACP) a_see_parser_capture(ACP,0)
#define CAPTURE_TEXT(ACP) a_see_parser_capture_text(ACP);
#define YYTEXT(acp)  a_see_parser_capture_text(ACP);
#endif
