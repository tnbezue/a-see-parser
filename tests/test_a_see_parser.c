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

#include "test_harness.h"
#include <a_see_parser/a_see_parser.h>

void test_init()
{

}

DECLARE_DEFAULT_A_SEE_PARSER;
IMPLEMENT_DEFAULT_A_SEE_PARSER;

void test_default()
{
  TESTCASE("Init");
  TEST("PTR null",__global_a_see_parser_pointer__->ptr_==NULL);
  TEST("Capture begin is  null",__global_a_see_parser_pointer__->capture_.begin_==NULL);
  TEST("Capture end is  null",__global_a_see_parser_pointer__->capture_.end_==NULL);
  TEST("FLAGS are default",__global_a_see_parser_pointer__->flags_ == (A_SEE_PARSER_DEFAULT_FLAGS));
}


typedef struct {
  const char* str;
  unsigned int len;
} string_length_t;

void test_c_comment()
{
  char msg[256];
  string_length_t test_data[] = {
    { " /* this\nis\na\ncomment */ this is not", 0 },
    { "/* this\nis\na\ncomment */ this is not" , 23},
    {  "/* this\nis/*\na\ncomment */ this is also */ but this isn't", 41},
    { NULL, 0}
  };
  TESTCASE("C Comment");
  string_length_t* isl = test_data;
  for(;isl->str;isl++) {
    SET_PARSE_STRING(isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" doesn't match",isl->str);
      TEST(msg,a_see_parser_c_comment(__global_a_see_parser_pointer__)==0);
    } else {
      sprintf(msg,"\"%s\" -- comment length is %d",isl->str,isl->len);
      TEST(msg,a_see_parser_c_comment(__global_a_see_parser_pointer__)==isl->len);
    }
  }
}

void test_cpp_comment()
{
  string_length_t test_data[] = {
    {" // this is a comment \nthis is not" , 0},
    {"// this is a comment \n this is not" , 22},
    { NULL, 0 },
  };
  char msg[256];
  TESTCASE("CPP Comment");
  string_length_t* isl = test_data;
  int nc=0;
  for(;isl->str;isl++) {
    SET_PARSE_STRING(isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" doesn't match",isl->str);
      TEST(msg,a_see_parser_cpp_comment(__global_a_see_parser_pointer__) == 0);
    } else {
      sprintf(msg,"\"%s\" -- comment length is %d",isl->str,isl->len);
      TEST(msg,(nc=a_see_parser_cpp_comment(__global_a_see_parser_pointer__)) == isl->len);
      printf("%d\n",nc);
    }
  }
}

void test_integer()
{
  string_length_t test_data[] = {
    { " 123" , 0 },
    {"123" , 3},
    { "12345678123213124123", 20 },
    { "123a", 3},
    { "0123", 0},
    { NULL, 0 },
  };
  char msg[256];
  TESTCASE("Integer");
  string_length_t* isl = test_data;
  for(;isl->str;isl++) {
    SET_PARSE_STRING(isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" fails",isl->str);
      TEST(msg,a_see_parser_decimal_integer(__global_a_see_parser_pointer__) == 0);
    } else {
      sprintf(msg,"\"%s\" len == %d",isl->str,isl->len);
      TEST(msg,a_see_parser_decimal_integer(__global_a_see_parser_pointer__));
    }
  }
}

void test_octal_integer()
{
  string_length_t test_data[] = {
    { " 0123", 0},
    { "0123", 4},
    { "01234567123213124123", 20 },
    { "01238", 4},
    { NULL, 0 },
  };
  char msg[256];
  TESTCASE("Octal Integer");
  string_length_t* isl = test_data;
  for(;isl->str;isl++) {
    SET_PARSE_STRING(isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" fails",isl->str);
      TEST(msg,a_see_parser_octal_integer(__global_a_see_parser_pointer__)==0);
    } else {
      sprintf(msg,"\"%s\" len == %d",isl->str,isl->len);
      TEST(msg,a_see_parser_octal_integer(__global_a_see_parser_pointer__));
    }
  }
}

void test_hex_integer()
{
  string_length_t test_data[] = {
    { " 0x123", 0 },
    { "0x123", 5 },
    { "0x12345678123213124123", 22 },
    { "0x123G", 5 },
    { "0xG123", 0 },
    { NULL, 0 },
  };
  char msg[256];
  TESTCASE("Hex Integer");
  string_length_t* isl = test_data;
  for(;isl->str;isl++) {
    SET_PARSE_STRING(isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" fails",isl->str);
      TEST(msg,a_see_parser_hex_integer(__global_a_see_parser_pointer__)==0);
    } else {
      sprintf(msg,"\"%s\" len == %d",isl->str,isl->len);
      TEST(msg,a_see_parser_hex_integer(__global_a_see_parser_pointer__));
    }
  }
}

void test_floating_point()
{
  string_length_t test_data[] = {
    { " 123.000", },
    { "123.0", 5},
    {  "1234567.", 8 },
    { "123", 0},
    { "123e", 0 },
    { "123e-", 0},
    { "123e1", 5 },
    { ".", 0},
    { "123e1x", 5},
    { ".0", 2},
    { NULL, 0 },
  };
  char msg[256];
  TESTCASE("Floating point");
  string_length_t* isl = test_data;
  for(;isl->str;isl++) {
    SET_PARSE_STRING(isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" fails",isl->str);
      TEST(msg,a_see_parser_floating_point(__global_a_see_parser_pointer__)==0);
    } else {
      sprintf(msg,"\"%s\" len == %d",isl->str,isl->len);
      TEST(msg,a_see_parser_floating_point(__global_a_see_parser_pointer__));
    }
  }
}

void test_quoted_string()
{
  string_length_t test_data[] = {
    { " \"This\" is a test" , 0},
    { "\"This is a test", 0 },
    { "\"This\" is a test", 6 },
    {"\"This\\\" is\" a test" , 11},
    { "\"This\\\" is\\\" a\" test", 15 },
    { "\"This\\\" is\\\" a\\n test\"", 22 },
    { "\"This\\\" is\\\" a\\n test", 0},
    { NULL, 0 },
  };
  TESTCASE("Quoted String");
  char msg[256];
  string_length_t* isl = test_data;
  for(;isl->str;isl++) {
    SET_PARSE_STRING(isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" -- doesn't match",isl->str);
      TEST(msg,a_see_parser_double_quoted_string(__global_a_see_parser_pointer__)==0);
    } else {
      sprintf(msg,"\"%s\" -- match length = %d",isl->str,isl->len);
      TEST(msg,a_see_parser_double_quoted_string(__global_a_see_parser_pointer__));
//      puts(YYTEXT(acp));
    }
  }
}

void test_next_chr()
{
  TESTCASE("NEXT_CHR");
  char msg[128];
  int test_data[] = { 'a','b','c',0,0,0};
  int n = ARRAY_SIZE(test_data);
  const char* str = "abc";
  SET_PARSE_STRING(str);
  int i;
  for(i=0;i<n;i++) {
    if(test_data[i])
      sprintf(msg,"NEXT_CHR is '%c'",test_data[i]);
    else
      sprintf(msg,"NEXT_CHR is end of string");
    TEST(msg,NEXT_CHR == test_data[i]);
  }
}

void test_peek_chr()
{
  TESTCASE("PEEK_CHR");
  char msg[128];
  int test_data[] = { 'a','b','c',0,0,0};
  int n = ARRAY_SIZE(test_data);
  const char* str = "abc";
  SET_PARSE_STRING(str);
  int i;
  for(i=0;i<n;i++) {
    if(test_data[i])
      sprintf(msg,"PEEK_CHR is '%c'",test_data[i]);
    else
      sprintf(msg,"PEEK_CHR is end of string");
    TEST(msg,PEEK_CHR == test_data[i]);
    NEXT_CHR;
  }
}

typedef struct {
  const char* range;
  const char* str;
  int found;
} string_string_int_t;
void test_range()
{
  string_string_int_t test_data[] = {
    { "-a-zA-Z0-9","-12",1 },
    { "-a-zA-Z0-9","12",1 },
    { "aBcDeFgHiJkLmNoPqRsTuVwXyZ","i12",1 },
    { "-a-zA-Z0-9","Kay",1 },
    { "-a-zA-Z0-9","+54",0 },
    { "aBcDeFgHiJkLmNoPqRsTuVwXyZ","33",0 },
    { NULL, NULL, 0}
  };
  TESTCASE("Range");
  char msg[128];
  string_string_int_t* issi = test_data;
  for(;issi->range;issi++) {
    SET_PARSE_STRING(issi->str);
    if(issi->found) {
      sprintf(msg,"Next char from \"%s\" found in \"%s\"",issi->str,issi->range);
      TEST(msg,a_see_parser_range(__global_a_see_parser_pointer__,issi->range));
    } else {
      sprintf(msg,"Next char from \"%s\" not found in \"%s\"",issi->str,issi->range);
      TEST(msg,a_see_parser_range(__global_a_see_parser_pointer__,issi->range)==0);
    }
  }
}

void test_not_range()
{
  string_string_int_t test_data[] = {
    { "^A-Z","12",1 },
    { "^0-9","12",0 },
    { "^aBcDeFgHiJkLmNoPqRsTuVwXyZ","i12",0 },
    { "^-a-zA-Z0-9","Kay",0 },
    { "^-a-zA-Z0-9","+54",1 },
    { "^aBcDeFgHiJkLmNoPqRsTuVwXyZ","33",1 },
    { NULL, NULL, 0}
  };
  TESTCASE("Range");
  char msg[128];
  string_string_int_t* issi = test_data;
  for(;issi->range;issi++) {
    SET_PARSE_STRING(issi->str);
    if(issi->found) {
      sprintf(msg,"Next char from \"%s\" not found in \"%s\"",issi->str,issi->range);
      TEST(msg,a_see_parser_range(__global_a_see_parser_pointer__,issi->range));
    } else {
      sprintf(msg,"Next char from \"%s\" found in \"%s\"",issi->str,issi->range);
      TEST(msg,a_see_parser_range(__global_a_see_parser_pointer__,issi->range)==0);
    }
  }
}

void test_sequence()
{
  string_string_int_t test_data[] = {
    { "abc","abcdefg",1 },
    { "abc","hijklmn",0 },
    { "o","opqrstu",1 },
    { "vwxyz01","vwxyz01",1 },
    { "345","2345678",0 },
    { NULL, NULL, 0}
  };
  TESTCASE("Sequence");
  char msg[128];
  string_string_int_t* issi = test_data;
  for(;issi->range;issi++) {
    SET_PARSE_STRING(issi->str);
    if(issi->found) {
      sprintf(msg,"Sequence \"%s\" found in  \"%s\"",issi->range,issi->str);
      TEST(msg,SEQUENCE(issi->range));
    } else {
      sprintf(msg,"Sequence \"%s\" not found in  \"%s\"",issi->range,issi->str);
      TEST(msg,SEQUENCE(issi->range)==0);
    }
  }
}

int simple_rule_test_1(int ch)
{
  return SIMPLE_RULE(NEXT_CHR == ch);
}

void test_simple_rule()
{
  TESTCASE("SIMPLE RULE");
  const char* str = "abc";
  SET_PARSE_STRING(str);
  int rc = simple_rule_test_1('b');
  TEST("Simple rule NEXT_CHR in \"abc\" equals 'b' fails",rc == 0 && __global_a_see_parser_pointer__->ptr_==str);
  rc = simple_rule_test_1('a');
  TEST("Simple rule NEXT_CHR in \"abc\" equals 'a' succeeds",rc == 1 && __global_a_see_parser_pointer__->ptr_==(str+1));
}

int true_false=0;
int rule_test_1(int ch)
{
  return RULE(NEXT_CHR==ch,true_false=1;,true_false=0;);
}

void test_rule()
{
  TESTCASE("RULE");
  const char* str = "abc";
  SET_PARSE_STRING(str);
  int rc = rule_test_1('b');
  TEST("Rule NEXT_CHR in \"abc\" equals 'b' fails",rc == 0 && __global_a_see_parser_pointer__->ptr_==str && true_false == 0);
  rc = rule_test_1('a');
  TEST("Rule NEXT_CHR in \"abc\" equals 'a' succeeds",rc == 1 && __global_a_see_parser_pointer__->ptr_==(str+1) && true_false == 1);
}

int count=0;
int zero_or_more_test_1(int ch)
{
  return ZERO_OR_MORE(NEXT_CHR==ch,count++;,);
}

typedef struct {
  int chr;
  int count;
} int_int_t;

void test_zero_or_more()
{
  TESTCASE("ZERO OR MORE");
  int_int_t test_data[] = {
    {'a',0},
    {'b',3},
    {'c',2},
    {'d',0},
    {'e',4}
  };
  char msg[128];
  int n = ARRAY_SIZE(test_data);
  const char* str = "bbbcceeee";
  SET_PARSE_STRING(str);
  int i;
  for(i=0;i<n;i++) {
    count = 0;
    sprintf(msg,"Find %d of '%c'",test_data[i].count,test_data[i].chr);
    TEST(msg,zero_or_more_test_1(test_data[i].chr) && count == test_data[i].count);
  }
}

int one_or_more_test_1(int ch)
{
  return ONE_OR_MORE(NEXT_CHR==ch,count++;,);
}

void test_one_or_more()
{
  TESTCASE("ONE OR MORE");
  int_int_t test_data[] = {
    {'a',0},
    {'b',3},
    {'c',2},
    {'d',0},
    {'e',4}
  };
  char msg[128];
  int n = ARRAY_SIZE(test_data);
  const char* str = "bbbcceeee";
  SET_PARSE_STRING(str);
  int i;
  for(i=0;i<n;i++) {
    count = 0;
    sprintf(msg,"Find %d of '%c'",test_data[i].count,test_data[i].chr);
    TEST(msg,one_or_more_test_1(test_data[i].chr) == (test_data[i].count>0) && count == test_data[i].count);
  }
}

int optional_1(int ch)
{
  return OPTIONAL(NEXT_CHR==ch,true_false=1;,true_false=0;);
}

void test_optional()
{
  TESTCASE("OPTIONAL");
  const char* str = "abc";
  SET_PARSE_STRING(str);
  TEST("Optional NEXT_CHR 'b' in \"abc\"",optional_1('b') && __global_a_see_parser_pointer__->ptr_==str && true_false == 0);
  TEST("Optional NEXT_CHR 'a' in \"abc\"",optional_1('a') && __global_a_see_parser_pointer__->ptr_==(str+1) && true_false == 1);
}

int non_consuming_test_1(int ch)
{
  return NON_CONSUMING_RULE(ZERO_OR_MORE(NEXT_CHR == ch,count++;,));
}

void test_non_consuming_rule()
{
  TESTCASE("Non consuming");
  const char* str = "aaaaa";
  SET_PARSE_STRING(str);
  TEST("5 characters consummed and pointer remains at beginning of string",
      non_consuming_test_1('a') && count == 5 && __global_a_see_parser_pointer__->ptr_==str);
}

test_function tests[] =
{
  test_default,
  test_c_comment,
  test_cpp_comment,
  test_integer,
  test_octal_integer,
  test_hex_integer,
  test_floating_point,
  test_quoted_string,
  test_range,
  test_not_range,
  test_next_chr,
  test_peek_chr,
  test_simple_rule,
  test_sequence,
  test_rule,
  test_zero_or_more,
  test_one_or_more,
  test_optional,
  test_non_consuming_rule,
};

TEST_MAIN(tests)
