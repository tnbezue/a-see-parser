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

/**
  Defining A_SEE_PARSER_LOCAL before including this file allows
  multiple parser instances and/or expanding parser structure
*/
#ifdef A_SEE_PARSER_LOCAL
# ifndef A_SEE_PARSER_STRUCTURE_EXTRA
#   define A_SEE_PARSER_STRUCTURE_EXTRA
# endif
#else
# define A_SEE_PARSER_STRUCTURE_EXTRA
#endif

/* Capture and A_SEE_PARSER structure */
typedef struct {
  const char* begin_;
  const char* end_;
} a_see_parser_capture_t;
typedef struct {
  const char* ptr_; // pointer to postion in parse string
  a_see_parser_capture_t capture_;
  A_SEE_PARSER_STRUCTURE_EXTRA
} a_see_parser_t;
#define A_SEE_PARSER_SAVE_STATE(ACP) a_see_parser_t __local_acp__ = *ACP;
#define A_SEE_PARSER_RESTORE_STATE(ACP) *ACP = __local_acp__

/* Basic A_SEE_PARSER predicates */
#define A_SEE_PARSER_PEEK_CHR(ACP) (*ACP->ptr_)
int a_see_parser_next_chr(a_see_parser_t*);
int a_see_parser_range(a_see_parser_t* acp,const char* range);
int a_see_parser_char_sequence(a_see_parser_t*,const char* include);
int a_see_parser_capture(a_see_parser_t*,int);
int a_see_parser_capture_length(a_see_parser_t*);
int a_see_parser_capture_text(a_see_parser_t*,char*,unsigned int);


/* Tracing.  Produces a lot of output */
#ifdef A_SEE_PARSER_TRACE
#define acp_trace if(1)
#define ACP_TRACE_RULE(code)  ({ {code} 1; })
#define a_see_parser_trace_info(ACP,RULE_NAME,STATUS) \
  fprintf(stderr,"%-8s %-30.30s %-20s %.30s\n",STATUS,__func__,# RULE_NAME,ACP->ptr_)
#define a_see_parser_trace_info_enter(ACP,RULE_NAME) a_see_parser_trace_info(ACP,RULE_NAME,"")
#define a_see_parser_trace_info_success(ACP,RULE_NAME) a_see_parser_trace_info(ACP,RULE_NAME,"Success ")
#define a_see_parser_trace_info_fail(ACP,RULE_NAME) a_see_parser_trace_info(ACP,RULE_NAME,"Fail ")
#else
#define acp_trace if(0)
#define ACP_TRACE_RULE(code) (1)
#define a_see_parser_trace_info_enter(ACP,RULE_NAME)
#define a_see_parser_trace_info_success(ACP,RULE_NAME)
#define a_see_parser_trace_info_fail(ACP,RULE_NAME)
#endif

#define A_SEE_PARSER_SET_PARSE_STRING(ACP,str) ACP->ptr_=str

#define A_SEE_PARSER_CAPTURE_ON(ACP) (ACP->capture_.begin_=ACP->ptr_,1)
#define A_SEE_PARSER_CAPTURE_OFF(ACP) (ACP->capture_.end_=ACP->ptr_,1)

/* Rule Directives */
#define A_SEE_PARSER_RULE(ACP,rule,action,cleanup) \
  ({ \
    A_SEE_PARSER_SAVE_STATE(ACP); \
    a_see_parser_trace_info_enter(ACP,RULE); \
    int __rC__ =0; \
    if(rule) {  __rC__=1; { action } a_see_parser_trace_info_success(ACP,RULE); } \
     else { { cleanup } a_see_parser_trace_info_fail(ACP,RULE); A_SEE_PARSER_RESTORE_STATE(ACP); } \
    __rC__; \
  })

#define A_SEE_PARSER_OPTIONAL(ACP,rule,action,cleanup) \
  ({ \
    A_SEE_PARSER_SAVE_STATE(ACP);  a_see_parser_trace_info_enter(ACP,OPTIONAL);\
    if(rule) {  { action } a_see_parser_trace_info_success(ACP,OPTIONAL);   } \
        else { { cleanup } a_see_parser_trace_info_fail(ACP,OPTIONAL); A_SEE_PARSER_RESTORE_STATE(ACP); } \
    1; \
  })

#define A_SEE_PARSER_ZERO_OR_MORE(ACP,rule,action,cleanup) \
  ({ \
    do { \
      A_SEE_PARSER_SAVE_STATE(ACP);  a_see_parser_trace_info_enter(ACP,ZERO_OR_MORE);\
      if( rule ) { { action } a_see_parser_trace_info_success(ACP,ZERO_OR_MORE); } \
      else { { cleanup} a_see_parser_trace_info_fail(ACP,ZERO_OR_MORE); A_SEE_PARSER_RESTORE_STATE(ACP); break; } \
    } while(1); \
    1; \
  })

#define A_SEE_PARSER_ONE_OR_MORE(ACP,rule,action,cleanup) \
  ({\
    int __cOuNt__=0;\
    do { \
      A_SEE_PARSER_SAVE_STATE(ACP);  a_see_parser_trace_info_enter(ACP,ONE_OR_MORE);\
      if(rule) { \
        __cOuNt__++; a_see_parser_trace_info_success(ACP,ONE_OR_MORE);\
        { action } \
      } else { \
        \
       { cleanup } a_see_parser_trace_info_fail(ACP,ONE_OR_MORE); \
         A_SEE_PARSER_RESTORE_STATE(ACP); break; \
      } \
    } while(1); \
    __cOuNt__>0; \
  })

#define A_SEE_PARSER_NON_CONSUMING_RULE(ACP,rule) \
  ({ \
    A_SEE_PARSER_SAVE_STATE(ACP); a_see_parser_trace_info_enter(ACP,NON_CONSUMING_RULE);\
    int __rC__=rule;\
    acp_trace { \
      if(__rC__) \
        a_see_parser_trace_info_success(ACP,NON_CONSUMING_RULE); \
      else \
        a_see_parser_trace_info_fail(ACP,NON_CONSUMING_RULE); \
    } \
    A_SEE_PARSER_RESTORE_STATE(ACP);\
    __rC__;\
  })

#define A_SEE_PARSER_NON_NOT(ACP,rule) \
  ({ \
    A_SEE_PARSER_SAVE_STATE(ACP); a_see_parser_trace_info_enter(ACP,NOT);\
    int __rC__=!(rule);\
    acp_trace { \
      if(__rC__) \
        a_see_parser_trace_info_success(ACP,NOT); \
      else \
        a_see_parser_trace_info_fail(ACP,NOT); \
    } \
    A_SEE_PARSER_RESTORE_STATE(ACP);\
    __rC__;\
  })

#define A_SEE_PARSER_SIMPLE_RULE(ACP,rule) \
      ({ A_SEE_PARSER_SAVE_STATE(ACP); a_see_parser_trace_info_enter(ACP,SIMPLE_RULE);int __Rc__ = rule; \
            if(__Rc__) { a_see_parser_trace_info_success(ACP,SIMPLE_RULE); } \
            else {  a_see_parser_trace_info_fail(ACP,SIMPLE_RULE); A_SEE_PARSER_RESTORE_STATE(ACP); } __Rc__; })

#define A_SEE_PARSER_CAPTURE_LENGTH(ACP) a_see_parser_capture_length(ACP)

#define A_SEE_PARSER_NEXT_CHR(ACP) a_see_parser_next_chr(ACP)
#define A_SEE_PARSER_NEXT_CHR_IS(ACP,CH) ((A_SEE_PARSER_PEEK_CHR(ACP) == CH) && a_see_parser_next_chr(ACP))
#define A_SEE_PARSER_NEXT_CHR_IS_NOT(ACP,CH) ((A_SEE_PARSER_PEEK_CHR(ACP) != CH) && a_see_parser_next_chr(ACP))
#define A_SEE_PARSER_ANY(ACP) a_see_parser_next_chr(ACP)

/* Common predicates. May be a bit more efficient that standard peg predicates*/
int a_see_parser_eol(a_see_parser_t*);
int a_see_parser_quoted_string(a_see_parser_t*,int);
#define a_see_parser_single_quoted_string(ACP) a_see_parser_quoted_string(ACP,'\'')
#define a_see_parser_double_quoted_string(ACP) a_see_parser_quoted_string(ACP,'"')
int a_see_parser_delimited_comment(a_see_parser_t*,const char* start,const char* end,int nested);
#define a_see_parser_c_comment(acp) a_see_parser_delimited_comment(acp,"/*","*/",0)
#define a_see_parser_nested_c_comment(acp) a_see_parser_delimited_comment(acp,"/*","*/",1)
#define a_see_parser_pascal_comment(acp) a_see_parser_delimited_comment(acp,"(*","*)",0)
#define a_see_parser_nested_pascal_comment(acp) a_see_parser_delimited_comment(acp,"(*","*)",1)
int a_see_parser_one_line_comment(a_see_parser_t*,const char* start);
#define a_see_parser_cpp_comment(acp) a_see_parser_one_line_comment(acp,"//")
#define a_see_parser_basic_comment(acp)  a_see_parser_one_line_comment(acp,"rem")
int a_see_parser_whitespace(a_see_parser_t*,const char*);
int a_see_parser_floating_point(a_see_parser_t*);
int a_see_parser_decimal_integer(a_see_parser_t*);
int a_see_parser_octal_integer(a_see_parser_t*);
int a_see_parser_hex_integer(a_see_parser_t*);
int a_see_parser_binary_integer(a_see_parser_t*);
int a_see_parser_ident(a_see_parser_t*);

//#define A_SEE_PARSER_WHITESPACE(ACP,WS_CHARS) a_see_parser_whitespace(ACP,WS_CHARS)

//#define A_SEE_PARSER_CHAR_SEQUENCE(ACP,SEQ) a_see_parser_char_sequence(ACP,SEQ)
//#define A_SEE_PARSER_RANGE(ACP,__RANGE__) a_see_parser_range(ACP,__RANGE__)

//#define A_SEE_PARSER_END_OF_LINE(ACP) a_see_parser_eol(ACP)


#define A_SEE_PARSER_DEFUALT { .ptr_=NULL, .capture_={.begin_=NULL,.end_=NULL } }

/* Multiple instances and/or extras */
#ifdef A_SEE_PARSER_LOCAL
#define DECLARE_A_SEE_PARSER(NAME) extern a_see_parser_t* NAME
#define IMPLEMENT_A_SEE_PARSER(NAME) static a_see_parser_t NAME ## _a_see_parser = A_SEE_PARSER_DEFUALT; \
    a_see_parser_t* NAME = &NAME ## _a_see_parser

#define ANY(ACP) A_SEE_PARSER_ANY(ACP)
#define SET_PARSE_STRING(ACP,str) A_SEE_PARSER_SET_PARSE_STRING(ACP,str)
#define NEXT_CHR(ACP) a_see_parser_next_chr(ACP)
#define NEXT_CHR_IS(ACP,CH) A_SEE_PARSER_NEXT_CHR_IS(ACP,CH)
#define NEXT_CHR_IS_NOT(ACP,CH) A_SEE_PARSER_NEXT_CHR_IS_NOT(ACP,CH)
#define PEEK_CHR(ACP) A_SEE_PARSER_PEEK_CHR(ACP)
#define RANGE(ACP,R) a_see_parser_range(ACP,R)
#define SEQUENCE(ACP,SEQ) a_see_parser_char_sequence(ACP,SEQ)
#define CAPTURE_ON(ACP) A_SEE_PARSER_CAPTURE_ON(ACP)
#define CAPTURE_OFF(ACP) A_SEE_PARSER_CAPTURE_OFF(ACP)
#define CAPTURE_LENGTH(ACP) A_SEE_PARSER_CAPTURE_LENGTH(ACP)
#define CAPTURE_TEXT(ACP,STR,LEN) a_see_parser_capture_text(ACP,STR,LEN)
#define RULE(ACP,rule,action,cleanup) A_SEE_PARSER_RULE(ACP,rule,action,cleanup)
#define SIMPLE_RULE(ACP,rule) A_SEE_PARSER_SIMPLE_RULE(ACP,rule)
#define ZERO_OR_MORE(ACP,rule,action,cleanup) A_SEE_PARSER_ZERO_OR_MORE(ACP,rule,action,cleanup)
#define ONE_OR_MORE(ACP,rule,action,cleanup) A_SEE_PARSER_ONE_OR_MORE(ACP,rule,action,cleanup)
#define OPTIONAL(ACP,rule,action,cleanup) A_SEE_PARSER_OPTIONAL(ACP,rule,action,cleanup)
#define NON_CONSUMING_RULE(ACP,rule) A_SEE_PARSER_NON_CONSUMING_RULE(ACP,rule)
#define NOT(ACP,rule) A_SEE_PARSER_NOT(ACP,rule)

#ifdef __USE_A_SEE_PARSER_PREDICATES__
#define ACP_EOL(ACP) a_see_parser_eol(ACP)
#define ACP_WHITESPACE(ACP,WS_CHARS)  a_see_parser_whitespace(ACP,WS_CHARS)
#define ACP_DOUBLE_QUOTED_STRING(ACP) a_see_parser_double_quoted_string(ACP)
#define ACP_SINGLE_QUOTED_STRING(ACP) a_see_parser_single_quoted_string(ACP)
#define ACP_QUOTED_STRING(ACP) (DOUBLE_QUOTED_STRING(ACP) || SINGLE_QUOTED_STRING(ACP))
#define ACP_FLOATING_POINT(ACP)a_see_parser_floating_point(ACP)
#define ACP_DECIMAL_INTEGER(ACP) a_see_parser_decimal_integer(ACP)
#define ACP_OCTAL_INTEGER(ACP) a_see_parser_octal_integer(ACP)
#define ACP_HEX_INTEGER(ACP)a_see_parser_hex_integer(ACP)
#define ACP_BINARY_INTEGER(ACP)a_see_parser_binary_integer(ACP)
#define ACP_IDENTIFIER(ACP) a_see_parser_ident(ACP)
#endif // __USE_A_SEE_PARSER_PREDICATES__

#else  // A_SEE_PARSER_LOCAL

/* Single instance and no extras */
#define DECLARE_DEFAULT_A_SEE_PARSER extern a_see_parser_t* __global
#define IMPLEMENT_DEFAULT_A_SEE_PARSER static a_see_parser_t __global_a_see_parser=A_SEE_PARSER_DEFUALT; \
    a_see_parser_t* __global = &__global_a_see_parser
#define SET_PARSE_STRING(str) A_SEE_PARSER_SET_PARSE_STRING(__global,str)
#define ANY A_SEE_PARSER_ANY(__global)
#define NEXT_CHR A_SEE_PARSER_NEXT_CHR(__global)
#define NEXT_CHR_IS(CH) A_SEE_PARSER_NEXT_CHR_IS(__global,CH)
#define NEXT_CHR_IS_NOT(CH) A_SEE_PARSER_NEXT_CHR_IS_NOT(__global,CH)
#define PEEK_CHR A_SEE_PARSER_PEEK_CHR(__global)
#define CAPTURE_ON A_SEE_PARSER_CAPTURE_ON(__global)
#define CAPTURE_OFF A_SEE_PARSER_CAPTURE_OFF(__global)
#define CAPTURE_LENGTH A_SEE_PARSER_CAPTURE_LENGTH(__global)
#define CAPTURE_TEXT(STR,LEN) a_see_parser_capture_text(__global,STR,LEN)
#define RANGE(R) a_see_parser_range(__global,R)
#define SEQUENCE(SEQ) a_see_parser_char_sequence(__global,SEQ)
#define RULE(rule,action,cleanup) A_SEE_PARSER_RULE(__global,rule,action,cleanup)
#define SIMPLE_RULE(rule) A_SEE_PARSER_SIMPLE_RULE(__global,rule)
#define ZERO_OR_MORE(rule,action,cleanup) A_SEE_PARSER_ZERO_OR_MORE(__global,rule,action,cleanup)
#define ONE_OR_MORE(rule,action,cleanup) A_SEE_PARSER_ONE_OR_MORE(__global,rule,action,cleanup)
#define OPTIONAL(rule,action,cleanup) A_SEE_PARSER_OPTIONAL(__global,rule,action,cleanup)
#define NON_CONSUMING_RULE(rule) A_SEE_PARSER_NON_CONSUMING_RULE(__global,rule)
#define NOT(rule) A_SEE_PARSER_NOT(__global,rule)

#ifdef __USE_A_SEE_PARSER_PREDICATES__
#define ACP_EOL a_see_parser_eol(__global)
#define ACP_WHITESPACE(WS_CHARS)  a_see_parser_whitespace(__global,WS_CHARS)
#define ACP_NESTED_C_COMMENT a_see_parser_nested_c_comment(__global)
#define ACP_C_COMMENT a_see_parser_c_comment(__global)
#define ACP_CPP_COMMENT a_see_parser_cpp_comment(__global)
#define ACP_BASIC_COMMENT  a_see_parser_basic_comment(__global)
#define ACP_FLOATING_POINT a_see_parser_floating_point(__global)
#define ACP_DECIMAL_INTEGER a_see_parser_decimal_integer(__global)
#define ACP_OCTAL_INTEGER a_see_parser_octal_integer(__global)
#define ACP_HEX_INTEGER a_see_parser_hex_integer(__global)
#define ACP_IDENTIFIER a_see_parser_ident(__global)
#define ACP_DOUBLE_QUOTED_STRING a_see_parser_double_quoted_string(__global)

#endif  // __USE_A_SEE_PARSER_PREDICATES__

#endif // A_SEE_PARSER_LOCAL


#endif // __A_SEE_PARSER_INCLUDED__
