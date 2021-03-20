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
#if 0
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
#endif

typedef struct {
  const char* begin_;
  const char* end_;
} a_see_parser_capture_t;
typedef struct {
  const char* ptr_; // pointer to postion in parse string
  a_see_parser_capture_t capture_;
  unsigned int line_number_;
  unsigned int flags_;
} a_see_parser_t;

#define ALLOW_NESTED_COMMENTS 1
#define ALLOW_EOL_IN_STRING 2
#define INCLUDE_EOL_IN_WHITESPACE 4
#define CASE_SENSITIVE 8

int a_see_parser_peek_chr(a_see_parser_t*);
int a_see_parser_next_chr(a_see_parser_t*);
int a_see_parser_whitespace(a_see_parser_t* acp,int include_new_line_as_whitespace);
int a_see_parser_eol(a_see_parser_t*);
int a_see_parser_range(a_see_parser_t* acp,const char* range);
int a_see_parser_quoted_string(a_see_parser_t*,int);
#define a_see_parser_single_quoted_string(ACP) a_see_parser_quoted_string(ACP,'\'')
#define a_see_parser_double_quoted_string(ACP) a_see_parser_quoted_string(ACP,'"')
int a_see_parser_delimited_comment(a_see_parser_t*,const char* start,const char* end,int nested);
#define a_see_parser_c_comment(acp) a_see_parser_delimited_comment(acp,"/*","*/",acp->flags_ & ALLOW_NESTED_COMMENTS)
#define a_see_parser_pascal_comment(acp) a_see_parser_delimited_comment(acp,"(*","*)",acp->flags_ & ALLOW_NESTED_COMMENTS)
int a_see_parser_one_line_comment(a_see_parser_t*,const char* start);
#define a_see_parser_cpp_comment(acp) a_see_parser_one_line_comment(acp,"//")
int a_see_parser_whitespace(a_see_parser_t*,int include_new_line_as_whitespace);
//int a_see_parser_space(a_see_parser_t*);
int a_see_parser_char_sequence(a_see_parser_t*,const char* include);
int a_see_parser_floating_point(a_see_parser_t*);
int a_see_parser_decimal_integer(a_see_parser_t*);
int a_see_parser_octal_integer(a_see_parser_t*);
int a_see_parser_hex_integer(a_see_parser_t*);
int a_see_parser_ident(a_see_parser_t*);
int a_see_parser_capture(a_see_parser_t*,int);
int a_see_parser_capture_length(a_see_parser_t*);
int a_see_parser_capture_text(a_see_parser_t*,char*,unsigned int);

#define A_SEE_PARSER_SET_PARSE_STRING(ACP,str) ACP->ptr_=str

#define A_SEE_PARSER_SAVE_STATE(ACP) a_see_parser_t __local_acp__ = *ACP;

#define A_SEE_PARSER_CAPTURE_ON(ACP) (ACP->capture_.begin_=ACP->ptr_,1)
#define A_SEE_PARSER_CAPTURE_OFF(ACP) (ACP->capture_.end_=ACP->ptr_,1)
#define A_SEE_PARSER_CAPTURE_LENGTH(ACP) a_see_parser_capture_length(ACP)

//#define A_SEE_PARSET_CAPTURE_LENGTY(ACP) A_SEE_PARSER_CAPTURE_LENGTH(ACP->capture_)
//#define A_SEE_PARSER_CURENT_CAPTURE(ACP->capture_,str,len) a_see_parser_capture_textACP->capture_
//#define A_SEE_PARSET_CAPTURE_TEXT(ACP->capture_,STR,LEN) a_see_parser_capture_text(ACP->capture_,STR,LEN)
#define A_SEE_PARSER_CAPTURE_TEXT(ACP,STR,LEN) a_see_parser_capture_text(ACP,STR,LEN)

#define A_SEE_PARSER_RESTORE_STATE(ACP) *ACP = __local_acp__

#define A_SEE_PARSER_PEEK_CHR(ACP) a_see_parser_peek_chr(ACP)
#define A_SEE_PARSER_NEXT_CHR(ACP) a_see_parser_next_chr(ACP)
#define A_SEE_PARSER_ANY(ACP) a_see_parser_next_chr(ACP)
#define A_SEE_PARSER_SPACE(ACP) \
 ({ while(a_see_parser_whitespace(ACP,ACP->flags_&INCLUDE_EOL_IN_WHITESPACE) \
    || a_see_parser_delimited_comment(ACP,"/*","*/",ACP->flags_ & ALLOW_NESTED_COMMENTS)\
    || a_see_parser_one_line_comment(ACP,"//")); 1; })

#define A_SEE_PARSER_CHAR_SEQUENCE(ACP,SEQ) a_see_parser_char_sequence(ACP,SEQ)
#define A_SEE_PARSER_RANGE(ACP,__RANGE__) a_see_parser_range(ACP,__RANGE__)

#define A_SEE_PARSER_END_OF_LINE(ACP) a_see_parser_eol(ACP)

#define A_SEE_PARSER_RULE(ACP,rule,action,cleanup) \
  ({ \
    A_SEE_PARSER_SAVE_STATE(ACP); \
    int __rC__ =0; \
    if(rule) {  __rC__=1; { action }  } \
     else { { cleanup } A_SEE_PARSER_RESTORE_STATE(ACP); } \
    __rC__; \
  })

#define A_SEE_PARSER_OPTIONAL(ACP,rule,action,cleanup) \
  ({ \
    A_SEE_PARSER_SAVE_STATE(ACP); \
    if(rule) { action  } else { { cleanup } A_SEE_PARSER_RESTORE_STATE(ACP); } \
    1; \
  })

#define A_SEE_PARSER_ZERO_OR_MORE(ACP,rule,action,cleanup) \
  ({ \
    do { \
      A_SEE_PARSER_SAVE_STATE(ACP); \
      if( rule ) { action } \
      else { { cleanup} A_SEE_PARSER_RESTORE_STATE(ACP); break; } \
    } while(1); \
    1; \
  })

#define A_SEE_PARSER_ONE_OR_MORE(ACP,rule,action,cleanup) \
  ({\
    int __cOuNt__=0;\
    do { \
      A_SEE_PARSER_SAVE_STATE(ACP); \
      if(rule) { \
        __cOuNt__++; \
        { action } \
      } else { \
        \
       { cleanup } A_SEE_PARSER_RESTORE_STATE(ACP); break; \
      } \
    } while(1); \
    __cOuNt__; \
  })

#define A_SEE_PARSER_SIMPLE_RULE(ACP,rule) \
      ({ A_SEE_PARSER_SAVE_STATE(ACP);int __Rc__ = rule; \
            if(!__Rc__) A_SEE_PARSER_RESTORE_STATE(ACP); __Rc__; })

#define A_SEE_PARSER_DEFAULT_FLAGS ALLOW_NESTED_COMMENTS | INCLUDE_EOL_IN_WHITESPACE

#define __A_SEE_PARSER_INIT__(ACP)({ \
  ACP = (a_see_parser_t){ .ptr_=NULL, .line_number_=1 ,.flags_ = A_SEE_PARSER_DEFAULT_FLAGS }; \
  &ACP; \
})

#define A_SEE_PARSER_DEFUALT { .ptr_=NULL, .capture_={.begin_=NULL,.end_=NULL } , \
        .line_number_=1 ,.flags_ = A_SEE_PARSER_DEFAULT_FLAGS}

#ifdef A_SEE_PARSER_LOCAL
#define DECLARE_A_SEE_PARSER(NAME) extern a_see_parser_t* NAME ## _parser
#define IMPLEMENT_A_SEE_PARSER(NAME) {static a_see_parser_t NAME ## _parser = A_SEE_PARSER_DEFUALT; \
    a_see_parser_t* NAME = &NAME ## _parser;}
#define SET_PARSE_STRING(ACP,str) A_SEE_PARSER_SET_PARSE_STRING(ACP,str)
#define NEXT_CHR(ACP) A_SEE_PARSER_NEXT_CHR(ACP)
#define ANY(ACP) A_SEE_PARSER_ANY(ACP)
#define PEEK_CHR(ACP) A_SEE_PARSER_PEEK_CHR(ACP)
#define CAPTURE_ON(ACP) A_SEE_PARSER_CAPTURE_ON(ACP)
#define CAPTURE_OF(ACP)F A_SEE_PARSER_CAPTURE_OFF(ACP)
#define CAPTURE_LENGTH(ACP) A_SEE_PARSER_CAPTURE_LENGTH(ACP)
//#define CURENT_CAPTURE(ACP) A_SEE_PARSER_CURENT_CAPTURE(ACP)
//#define CAPTURE_LENGTH(ACP) A_SEE_PARSER_CURENT_CAPTURE_LENGTH(ACP)
#define RULE(ACP,rule,action,cleanup) A_SEE_PARSER_RULE(ACP,rule,action,cleanup)
#define SIMPLE_RULE(ACP,rule) A_SEE_PARSER_SIMPLE_RULE(ACP,rule)
#define ZERO_OR_MORE(ACP,rule,action,cleanup) A_SEE_PARSER_ZERO_OR_MORE(ACP,rule,action,cleanup)
#define ONE_OR_MORE(ACP,rule,action,cleanup) A_SEE_PARSER_ONE_OR_MORE(ACP,rule,action,cleanup)
#define OPTIONAL(ACP,rule,action,cleanup) A_SEE_PARSER_OPTIONAL(ACP,rule,action,cleanup)
#define REAL(ACP)a_see_parser_floating_point(ACP)
#define DECIMAL_INTEGER(ACP) a_see_parser_decimal_integer(ACP)
#define OCTAL_INTEGER(ACP) a_see_parser_octal_integer(ACP)
#define HEX_INTEGER(ACP)a_see_parser_hex_integer(ACP)
#define IDENTIFIER(ACP) a_see_parser_ident(ACP)
#define SPACE(ACP) A_SEE_PARSER_SPACE(ACP)
#else
#define DECLARE_DEFAULT_A_SEE_PARSER extern a_see_parser_t* __global_a_see_parser_pointer__
#define IMPLEMENT_DEFAULT_A_SEE_PARSER static a_see_parser_t __global_a_see_parser_pointer_parser=A_SEE_PARSER_DEFUALT; \
    a_see_parser_t* __global_a_see_parser_pointer__ = &__global_a_see_parser_pointer_parser
#define SET_PARSE_STRING(str) A_SEE_PARSER_SET_PARSE_STRING(__global_a_see_parser_pointer__,str)
#define NEXT_CHR A_SEE_PARSER_NEXT_CHR(__global_a_see_parser_pointer__)
#define ANY A_SEE_PARSER_ANY(__global_a_see_parser_pointer__)
#define PEEK_CHR A_SEE_PARSER_PEEK_CHR(__global_a_see_parser_pointer__)
#define CAPTURE_ON A_SEE_PARSER_CAPTURE_ON(__global_a_see_parser_pointer__)
#define CAPTURE_OFF A_SEE_PARSER_CAPTURE_OFF(__global_a_see_parser_pointer__)
//#define CURRENT_CAPTURE A_SEE_PARSER_CURENT_CAPTURE(__global_a_see_parser_pointer__)
#define CAPTURE_LENGTH A_SEE_PARSER_CAPTURE_LENGTH(__global_a_see_parser_pointer__)
#define CAPTURE_TEXT(STR,LEN) A_SEE_PARSER_CAPTURE_TEXT(__global_a_see_parser_pointer__,STR,LEN)
//#define CURRENT_CAPTURE_LENGTH A_SEE_PARSER_CURENT_CAPTURE_LENGTH(__global_a_see_parser_pointer__)
#define RULE(rule,action,cleanup) A_SEE_PARSER_RULE(__global_a_see_parser_pointer__,rule,action,cleanup)
#define SIMPLE_RULE(rule) A_SEE_PARSER_SIMPLE_RULE(__global_a_see_parser_pointer__,rule)
#define ZERO_OR_MORE(rule,action,cleanup) A_SEE_PARSER_ZERO_OR_MORE(__global_a_see_parser_pointer__,rule,action,cleanup)
#define ONE_OR_MORE(rule,action,cleanup) A_SEE_PARSER_ONE_OR_MORE(__global_a_see_parser_pointer__,rule,action,cleanup)
#define OPTIONAL(rule,action,cleanup) A_SEE_PARSER_OPTIONAL(__global_a_see_parser_pointer__,rule,action,cleanup)
#define REAL a_see_parser_floating_point(__global_a_see_parser_pointer__)
#define DECIMAL_INTEGER a_see_parser_decimal_integer(__global_a_see_parser_pointer__)
#define OCTAL_INTEGER a_see_parser_octal_integer(__global_a_see_parser_pointer__)
#define HEX_INTEGER a_see_parser_hex_integer(__global_a_see_parser_pointer__)
#define IDENTIFIER a_see_parser_ident(__global_a_see_parser_pointer__)
#define SPACE A_SEE_PARSER_SPACE(__global_a_see_parser_pointer__)
extern a_see_parser_t* __global_a_see_parser_pointer__;
#endif

#if 0
a_see_parser_t __global_a_see_parser__ = { ptr_=NULL, .capture_begin_ = NULL,
    capture_end_=NULL, .line_number_=1 ,.flags = A_SEE_PARSER_DEFAULT_FLAGS};
void a_see_parser_set_string(a_see_parser_t*,const char*);


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
    __cOuNt__; \
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
    int __rC__=rule;\
    a_see_parser_rule_fail(ACP);\
    __rC__;\
  })

#define RANGE(ACP,str) a_see_parser_range(acp,str)
#define CAPTURE_ON(ACP) a_see_parser_capture(ACP,1)
#define CAPTURE_OFF(ACP) a_see_parser_capture(ACP,0)
#define CAPTURE_TEXT(ACP) a_see_parser_capture_text(ACP)
#define YYTEXT(ACP)  a_see_parser_capture_text(ACP)
#endif

#endif
