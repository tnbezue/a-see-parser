#include "test_harness.h"
#include <a_see_parser/a_see_parser.h>

void test_init()
{

}

void test_new_delete()
{
  TESTCASE("New/Delete");
  a_see_parser_t* acp = new_a_see_parser();
  TEST("PTR null",acp->ptr_==NULL);
  TEST("Stack size zero",simple_array_size(acp->stack_)==0);
  TEST("Text size zero",simple_array_size(acp->text_)==0);
  delete_a_see_parser(acp);
}

typedef struct {
  const char* str;
  unsigned int len;
} string_length_t;

void test_c_comment()
{
  char msg[256];
  string_length_t test_data[] = {
//    { " /* this\nis\na\ncomment */ this is not", 0 },
    { "/* this\nis\na\ncomment */ this is not" , 23},
//    {  "/* this\nis/*\na\ncomment */ this is also */ but this isn't", 41},
    { NULL, 0}
  };
  TESTCASE("C Comment");
  a_see_parser_t* acp = new_a_see_parser();
  string_length_t* isl = test_data;
  for(;isl->str;isl++) {
    a_see_parser_set_string(acp,isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" doesn't match",isl->str);
      TEST(msg,a_see_parser_c_comment(acp)==0);
    } else {
      sprintf(msg,"\"%s\" -- comment length is %d",isl->str,isl->len);
      TEST(msg,a_see_parser_c_comment(acp)==isl->len);
    }
  }
  delete_a_see_parser(acp);
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
  a_see_parser_t* acp = new_a_see_parser();
  string_length_t* isl = test_data;
  int nc=0;
  for(;isl->str;isl++) {
    a_see_parser_set_string(acp,isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" doesn't match",isl->str);
      TEST(msg,a_see_parser_cpp_comment(acp) == 0);
    } else {
      sprintf(msg,"\"%s\" -- comment length is %d",isl->str,isl->len);
      TEST(msg,(nc=a_see_parser_cpp_comment(acp)) == isl->len);
      printf("%d\n",nc);
    }
  }
  delete_a_see_parser(acp);
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
  a_see_parser_t* acp = new_a_see_parser();
  string_string_int_t* issi = test_data;
  for(;issi->range;issi++) {
    a_see_parser_set_string(acp,issi->str);
    if(issi->found) {
      sprintf(msg,"Next char from \"%s\" found in \"%s\"",issi->str,issi->range);
      TEST(msg,a_see_parser_range(acp,issi->range));
    } else {
      sprintf(msg,"Next char from \"%s\" not found in \"%s\"",issi->str,issi->range);
      TEST(msg,a_see_parser_range(acp,issi->range)==0);
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
  a_see_parser_t* acp = new_a_see_parser();
  for(;isl->str;isl++) {
    a_see_parser_set_string(acp,isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" fails",isl->str);
      TEST(msg,a_see_parser_decimal_integer(acp) == 0);
    } else {
      sprintf(msg,"\"%s\" len == %d",isl->str,isl->len);
      TEST(msg,a_see_parser_decimal_integer(acp));
    }
  }
  delete_a_see_parser(acp);
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
  a_see_parser_t* acp = new_a_see_parser();
  string_length_t* isl = test_data;
  for(;isl->str;isl++) {
    a_see_parser_set_string(acp,isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" fails",isl->str);
      TEST(msg,a_see_parser_octal_integer(acp)==0);
    } else {
      sprintf(msg,"\"%s\" len == %d",isl->str,isl->len);
      TEST(msg,a_see_parser_octal_integer(acp));
    }
  }
  delete_a_see_parser(acp);
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
  a_see_parser_t* acp = new_a_see_parser();
  string_length_t* isl = test_data;
  for(;isl->str;isl++) {
    a_see_parser_set_string(acp,isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" fails",isl->str);
      TEST(msg,a_see_parser_hex_integer(acp)==0);
    } else {
      sprintf(msg,"\"%s\" len == %d",isl->str,isl->len);
      TEST(msg,a_see_parser_hex_integer(acp));
    }
  }
  delete_a_see_parser(acp);
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
  a_see_parser_t* acp = new_a_see_parser();
  for(;isl->str;isl++) {
    a_see_parser_set_string(acp,isl->str);
    if(isl->len == 0) {
      sprintf(msg,"\"%s\" fails",isl->str);
      TEST(msg,a_see_parser_floating_point(acp)==0);
    } else {
      sprintf(msg,"\"%s\" len == %d",isl->str,isl->len);
      TEST(msg,a_see_parser_floating_point(acp));
    }
  }
  delete_a_see_parser(acp);
}

test_function tests[] =
{
  test_new_delete,
  test_c_comment,
  test_cpp_comment,
  test_range,
  test_integer,
  test_octal_integer,
  test_hex_integer,
  test_floating_point,
};

TEST_MAIN(tests)
