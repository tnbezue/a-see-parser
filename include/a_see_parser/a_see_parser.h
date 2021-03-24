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

//int a_see_parser_peek_chr(a_see_parser_t*);
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

#define A_SEE_PARSER_SAVE_STATE(ACP) a_see_parser_t __local_acp__ = *ACP;
#define A_SEE_PARSER_RESTORE_STATE(ACP) *ACP = __local_acp__

#define A_SEE_PARSER_CAPTURE_ON(ACP) (ACP->capture_.begin_=ACP->ptr_,1)
#define A_SEE_PARSER_CAPTURE_OFF(ACP) (ACP->capture_.end_=ACP->ptr_,1)
#define A_SEE_PARSER_CAPTURE_LENGTH(ACP) a_see_parser_capture_length(ACP)

//#define A_SEE_PARSET_CAPTURE_LENGTY(ACP) A_SEE_PARSER_CAPTURE_LENGTH(ACP->capture_)
//#define A_SEE_PARSER_CURENT_CAPTURE(ACP->capture_,str,len) a_see_parser_capture_textACP->capture_
//#define A_SEE_PARSET_CAPTURE_TEXT(ACP->capture_,STR,LEN) a_see_parser_capture_text(ACP->capture_,STR,LEN)
#define A_SEE_PARSER_CAPTURE_TEXT(ACP,STR,LEN) a_see_parser_capture_text(ACP,STR,LEN)

#define A_SEE_PARSER_PEEK_CHR(ACP) (*ACP->ptr_)
#define A_SEE_PARSER_NEXT_CHR(ACP) a_see_parser_next_chr(ACP)
#define A_SEE_PARSER_NEXT_CHR_IS(ACP,CH) ((A_SEE_PARSER_PEEK_CHR(ACP) == CH) && A_SEE_PARSER_NEXT_CHR(ACP))
#define A_SEE_PARSER_NEXT_CHR_IS_NOT(ACP,CH) ((A_SEE_PARSER_PEEK_CHR(ACP) != CH) && A_SEE_PARSER_NEXT_CHR(ACP))
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
#define NEXT_CHR_IS(ACP,CH) A_SEE_PARSER_NEXT_CHR_IS(ACP,CH)
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
#define SEQUENCE(ACP,SEQ) a_see_parser_char_sequence(ACP,SEQ)
#define NON_CONSUMING_RULE(ACP,rule) A_SEE_PARSER_NON_CONSUMING_RULE(ACP,rule)
#define NOT(ACP,rule) A_SEE_PARSER_NOT(ACP,rule)
#define NEXT_CHR_IS_NOT(ACP,CH) A_SEE_PARSER_NEXT_CHR_IS_NOT(ACP,CH)
#define SPACE(ACP) A_SEE_PARSER_SPACE(ACP)
#define RANGE(ACP,__RANGE__) a_see_parser_range(ACP,__RANGE__)
#define DOUBLE_QUOTED_STRING(ACP) a_see_parser_double_quoted_string(ACP)
#define EOL(ACP) a_see_parser_eol(ACP)
#else
#define DECLARE_DEFAULT_A_SEE_PARSER extern a_see_parser_t* __global_a_see_parser_pointer__
#define IMPLEMENT_DEFAULT_A_SEE_PARSER static a_see_parser_t __global_a_see_parser_pointer_parser=A_SEE_PARSER_DEFUALT; \
    a_see_parser_t* __global_a_see_parser_pointer__ = &__global_a_see_parser_pointer_parser
#define SET_PARSE_STRING(str) A_SEE_PARSER_SET_PARSE_STRING(__global_a_see_parser_pointer__,str)
#define NEXT_CHR A_SEE_PARSER_NEXT_CHR(__global_a_see_parser_pointer__)
#define NEXT_CHR_IS(CH) A_SEE_PARSER_NEXT_CHR_IS(__global_a_see_parser_pointer__,CH)
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
#define SEQUENCE(SEQ) a_see_parser_char_sequence(__global_a_see_parser_pointer__,SEQ)
#define IDENTIFIER a_see_parser_ident(__global_a_see_parser_pointer__)
#define NON_CONSUMING_RULE(rule) A_SEE_PARSER_NON_CONSUMING_RULE(__global_a_see_parser_pointer__,rule)
#define NOT(rule) A_SEE_PARSER_NOT(__global_a_see_parser_pointer__,rule)
#define NEXT_CHR_IS_NOT(CH) A_SEE_PARSER_NEXT_CHR_IS_NOT(__global_a_see_parser_pointer__,CH)
#define SPACE A_SEE_PARSER_SPACE(__global_a_see_parser_pointer__)
#define RANGE(__RANGE__) a_see_parser_range(__global_a_see_parser_pointer__,__RANGE__)
#define DOUBLE_QUOTED_STRING a_see_parser_double_quoted_string(__global_a_see_parser_pointer__)
extern a_see_parser_t* __global_a_see_parser_pointer__;
#define EOL a_see_parser_eol(__global_a_see_parser_pointer__)
#endif


#endif
