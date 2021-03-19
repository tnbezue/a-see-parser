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

test_function tests[] =
{
  test_default,
  test_c_comment,
  test_cpp_comment,
  test_range,
  test_integer,
  test_octal_integer,
  test_hex_integer,
  test_floating_point,
  test_quoted_string,
};

TEST_MAIN(tests)
