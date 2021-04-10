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
  }
  return c;
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

int a_see_parser_char_sequence(a_see_parser_t*acp,const char*str)
{
  const char* ptr=acp->ptr_;
  for(;*str && *ptr && *str == *ptr;str++,ptr++);
  int rc=0;
  if(*str==0) {
    acp->ptr_=ptr;
    rc=1;
  }
  return rc;
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

int a_see_parser_whitespace(a_see_parser_t* acp,const char* ws_chars)
{
  const char* ptr=acp->ptr_;
  for(;*ptr && strchr(ws_chars,*ptr);ptr++);
  int rc=0;
  if(ptr > acp->ptr_) {
    rc = 1;
    acp->ptr_=ptr;
  }
  return rc;
}

int a_see_parser_eol(a_see_parser_t* acp)
{
  int rc = 0;
  int c=A_SEE_PARSER_PEEK_CHR(acp);
  if(c=='\r') {
    // Mac or windows
    rc = 1;
    a_see_parser_next_chr(acp);
    c=A_SEE_PARSER_PEEK_CHR(acp);
    if(c == '\n') // it's WINDOWS
       a_see_parser_next_chr(acp);
  } else if(c=='\n') { // Linux or Unix
    a_see_parser_next_chr(acp);
    rc = 1;
  }
  return rc;
}

int a_see_parser_delimited_comment(a_see_parser_t*acp,const char* start,const char* end,int nested)
{
  int len=0;
  const char* ptr = acp->ptr_;
  const char* start_temp = start;
  for(;*start_temp && *ptr && *start_temp == *ptr;start_temp++,ptr++);
  if(*start_temp == 0) {
    int level=1;
    while(*ptr && level) {
      const char* ptr_temp = ptr;
      const char* end_temp = end;
      for(;*end_temp && *ptr && *end_temp == *ptr;end_temp++,ptr++);
      if(*end_temp == 0) {
        level--;
      } else if(nested) {
        for(ptr=ptr_temp,start_temp=start;*start_temp && *ptr && *start_temp==*ptr;start_temp++,ptr++);
        if(*start_temp == 0) {
          level++;
        } else {
          ptr = ptr_temp+1;
        }
      } else {
          ptr = ptr_temp+1;
      }
    }
    if(level == 0) {
      len = ptr - acp->ptr_;
      acp->ptr_=ptr;
    }
  }
  return len;
}

int a_see_parser_one_line_comment(a_see_parser_t*acp,const char* start)
{
  int len=0;
  const char* ptr = acp->ptr_;
  for(;*start && *ptr && *start == *ptr;start++,ptr++);
  /* if the last character in sequence if alpha numeric, then next character must be non alpha numeric */
  if(*start == 0) {
    if(!isalnum(*(start-1)) || !(isalnum(*ptr) || *ptr == '_')) {
      for(;*ptr && *ptr != '\r' && *ptr != '\n';ptr++);
    }
    len=ptr-acp->ptr_;
    acp->ptr_=ptr;
  }
  return len;
}

int a_see_parser_floating_point(a_see_parser_t*acp)
{
  const char* ptr = acp->ptr_;
  int has_whole = 0;
  int has_decimal_point = 0;
  int has_decimal_digits=0;
  int has_exponent=0; // 0 - not specified, -1 -- e (or E) without digits, 1 -- valid
  for(;(*ptr >= '0' && *ptr <= '9');has_whole = 1,ptr++);
  if (*ptr == '.') {
    has_decimal_point = 1;
    for(ptr++;(*ptr >= '0' && *ptr <= '9');ptr++,has_decimal_digits=1);
  }
  if(has_whole || has_decimal_digits ) {
    if(*ptr == 'e' || *ptr == 'E') {
      has_exponent = -1;
      ptr++;
      if(*ptr == '+' || *ptr == '-') {
        ptr++;
      }
      for(;(*ptr >= '0' && *ptr <= '9');ptr++,has_exponent=1);
    }
  }
  int rc = (has_exponent >= 0) && ((has_whole && has_exponent)
        || (has_whole && has_decimal_point) || has_decimal_digits);
  if(rc) {
    acp->capture_.begin_=acp->ptr_;
    acp->ptr_=ptr;
    acp->capture_.end_=acp->ptr_;
  }
  return rc;
}

int a_see_parser_decimal_integer(a_see_parser_t*acp)
{
  const char* ptr = acp->ptr_;
  int rc=0;
  if(*ptr >= '1' && *ptr <= '9') {
    rc=1;
    for(ptr++;*ptr >= '0' && *ptr <= '9';ptr++);
    acp->capture_.begin_=acp->ptr_;
    acp->ptr_=ptr;
    acp->capture_.end_=acp->ptr_;
  }
  return rc;
}

int a_see_parser_octal_integer(a_see_parser_t*acp)
{
  const char* ptr = acp->ptr_;
  int rc=0;
  if(*ptr == '0') {
    rc=1;
    for(ptr++;*ptr >= '0' && *ptr <= '7';ptr++);
    acp->capture_.begin_=acp->ptr_;
    acp->ptr_=ptr;
    acp->capture_.end_=acp->ptr_;
  }
  return rc;
}

int a_see_parser_hex_integer(a_see_parser_t*acp)
{
  const char* ptr = acp->ptr_;
  int len=0;
  if(*ptr == '0' && toupper(*(ptr+1)) == 'X') {
    for(ptr+=2;(*ptr >= '0' && *ptr <= '9') ||
          (toupper(*ptr) >= 'A' && toupper(*ptr) <= 'F');ptr++);
    len = ptr - acp->ptr_;
    if(len > 2) {
      acp->capture_.begin_=acp->ptr_;
      acp->ptr_=ptr;
      acp->capture_.end_=acp->ptr_;
    } else
      len=0;
  }
  return len;
}

int a_see_parser_binary_integer(a_see_parser_t*acp)
{
  const char* ptr = acp->ptr_;
  int len=0;
  if(*ptr == '0' && toupper(*(ptr+1)) == 'B') {
    for(ptr+=2;(*ptr == '0' || *ptr == '1');ptr++);
    len = ptr - acp->ptr_;
    if(len > 2) {
      acp->capture_.begin_=acp->ptr_;
      acp->ptr_=ptr;
      acp->capture_.end_=acp->ptr_;
    } else
      len=0;
  }
  return len;
}

int a_see_parser_ident(a_see_parser_t* acp)
{
  const char* ptr = acp->ptr_;
  int rc=0;
  if((*ptr >= 'a' && *ptr <= 'z') || *ptr == '_' || (*ptr >= 'A' && *ptr <= 'Z')) {
    for(ptr++;(*ptr >= 'a' && *ptr <= 'z') || *ptr == '_' ||
          (*ptr >= '0' && *ptr <= '9') || (*ptr >= 'A' && *ptr <= 'Z');ptr++);
    acp->capture_.begin_=acp->ptr_;
    acp->ptr_=ptr;
    acp->capture_.end_=acp->ptr_;
    rc = 1;
  }
  return rc;
}

/*
  Allow for escaped characters.
*/
int a_see_parser_quoted_string(a_see_parser_t* acp,int ch)
{
  const char* ptr = acp->ptr_;
  int rc=0;
  if(*ptr == ch) {
    for(ptr++;*ptr;ptr++) {
      if(*ptr == '\\') ptr++;
      else if(*ptr == ch)
        break;
    }
    if(*ptr) {
      acp->capture_.begin_=acp->ptr_;
      acp->ptr_=ptr;
      acp->capture_.end_=acp->ptr_;
      rc = 1;
    }
  }
  return rc;
}
