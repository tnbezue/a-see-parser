#include <a_see_parser/a_see_parser.h>
#include <string.h>
#include <stdio.h>

void dynamic_string_ctor(dynamic_string_t* ds)
{
  ds->text_=NULL;
  ds->length_=0;
}

void dynamic_string_dtor(dynamic_string_t* ds)
{
  if(ds->text_)
    free(ds->text_);
}

void a_see_parser_init(a_see_parser_t* acp)
{
  acp->stack_ = new_simple_array(sizeof(a_see_parser_position_t),NULL,NULL,NULL,NULL);
  acp->text_ = new_dynamic_string();
  acp->ptr_=NULL;
  acp->line_number_=0;
  acp->flags_=A_SEE_PARSER_DEFAULT_FLAGS;
}

void a_see_parser_dtor(a_see_parser_t* acp)
{
  delete_simple_array(acp->stack_);
  delete_dynamic_string(acp->text_);
}

void dynamic_string_set(dynamic_string_t* ds,const char* str,unsigned int length)
{
  if(str) {
    if(ds->length_ <= length) {
      if(ds->text_)
        free(ds->text_);
      ds->length_=length+1;
      ds->text_ = malloc(ds->length_);
      strncpy(ds->text_,str,length);
    }
  }
}

void a_see_parser_reset(a_see_parser_t* acp)
{
  simple_array_clear(acp->stack_);
}

void a_see_parser_set_string(a_see_parser_t* acp,const char* str)
{
  acp->ptr_ = str;
  acp->line_number_=1;
}

void a_see_parser_rule_start(a_see_parser_t* acp)
{
  a_see_parser_position_t pos = { .ptr_ = acp->ptr_, .line_number_= acp->line_number_};
  simple_array_push_back(acp->stack_,&pos);
}

void a_see_parser_rule_success(a_see_parser_t* acp)
{
  simple_array_pop_back(acp->stack_);
}

void a_see_parser_rule_fail(a_see_parser_t* acp)
{
  a_see_parser_size_t s = simple_array_size(acp->stack_);
  if(s) {
    a_see_parser_position_t* pp = simple_array_at(acp->stack_,s-1);
    acp->ptr_=pp->ptr_;
    acp->line_number_=pp->line_number_;
    simple_array_pop_back(acp->stack_);
  }
}

int a_see_parser_peek_chr(a_see_parser_t* acp)
{
  int c = *acp->ptr_;
  return c;
}

int a_see_parser_next_chr(a_see_parser_t* acp)
{
  int c = a_see_parser_peek_chr(acp);
  if(c) {
    acp->ptr_++;
    if(c=='\n')
      acp->line_number_++;
  }
  return c;
}
/*
int a_see_parser_current_chr(a_see_parser_t* acp)
{

}*/

static const char* whitespace_chars = " \t";
static const char* whitespace_chars_with_eol = " \t\r\n";

int a_see_parser_whitespace(a_see_parser_t* acp,int include_new_line_as_whitespace)
{
  const char* ws_chars = include_new_line_as_whitespace ? whitespace_chars_with_eol : whitespace_chars;
  int c,nchar=0;
  while((c=a_see_parser_peek_chr(acp))) {
    if(strchr(ws_chars,c)==NULL)
      break;
    nchar++;
    a_see_parser_next_chr(acp);
  }
  return nchar;
}

int a_see_parser_eol(a_see_parser_t* acp)
{
  int c;
  return SIMPLE_RULE(acp,NEXT_CHR(acp) == '\r' && NEXT_CHR(acp) == '\n')
    ||
    SIMPLE_RULE(acp,(c=NEXT_CHR(acp)) == '\r' || c == '\n');
}

int a_see_parser_char_sequence(a_see_parser_t*acp,const char*str)
{
  return SIMPLE_RULE(acp,
    ({
      int rc=1;
      int c;
      while(*str && (c=NEXT_CHR(acp))) {
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
  SIMPLE_RULE(acp,
      SEQUENCE(acp,start) && ZERO_OR_MORE(acp,!SEQUENCE(acp,end) &&
        OPTIONAL(acp,nested && a_see_parser_delimited_comment(acp,start,end,nested),,) &&
        NEXT_CHR(acp),,) && SEQUENCE(acp,end)
  );
  len = acp->ptr_-ptr;
  return len;
}

int a_see_parser_one_line_comment(a_see_parser_t*acp,const char* start)
{
  int len=0;
  const char* ptr = acp->ptr_;
  SIMPLE_RULE(acp,SEQUENCE(acp,start) && ZERO_OR_MORE(acp,!END_OF_LINE(acp) && NEXT_CHR(acp),,)
    && END_OF_LINE(acp)
  );
  len=acp->ptr_-ptr;
  return len;
}

int a_see_parser_space(a_see_parser_t* acp)
{
  ZERO_OR_MORE(acp,a_see_parser_whitespace(acp,acp->flags_&INCLUDE_EOL_IN_WHITESPACE)
    || a_see_parser_c_comment(acp) || a_see_parser_cpp_comment(acp),,);
  return 1;
}

int a_see_parser_capture(a_see_parser_t* acp,int on_off)
{
  if(on_off) {
    acp->capture_begin_ = acp->ptr_;
    acp->capture_end_ = NULL;
  } else {
    acp->capture_end_ = acp->ptr_;
  }
  return 1;
}

const char* a_see_parser_capture_text(a_see_parser_t* acp)
{
  if(acp->capture_begin_ && acp->capture_end_ && acp->capture_end_ >= acp->capture_begin_) {
    dynamic_string_set(acp->text_,acp->capture_begin_,acp->capture_end_-acp->capture_begin_);
  } else
    dynamic_string_set(acp->text_,"",0);
  return acp->text_->text_;
}

// Works like [-a-zA-Z0-9]
int a_see_parser_range(a_see_parser_t* acp,const char* range)
{
  int rc=0;
  int c = PEEK_CHR(acp);
  int fc;
  for(;*range;range++) {
    fc=*range;
    if(*(range+1)== '-') {
      range+=2;
      if(*range) {
        if(fc <= c && c <= *range) {
          rc = c;
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
  if(rc)
    NEXT_CHR(acp);
  return rc;
/*
  return SIMPLE_RULE(acp,
  ({
    int rc=0;
    int c = NEXT_CHR(acp);
    int fc;
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
          rc=1;
          break;
        }
      }
    }
    rc;
  })
  );*/
}

// < ([0-9]+ ('.' [0-9]*)? [eE] [-+]? [0-9]+) | (// [0-9]* '.' [0-9]+ ([eE] [-+]? [0-9]+)?) >
int a_see_parser_whole(a_see_parser_t* acp)
{
  return SIMPLE_RULE(acp,ONE_OR_MORE(acp,RANGE(acp,"0-9"),,));
}

int a_see_parser_fraction(a_see_parser_t* acp)
{
  return SIMPLE_RULE(acp,NEXT_CHR(acp) == '.' && ZERO_OR_MORE(acp,RANGE(acp,"0-9"),,));
}

int a_see_parser_exponent(a_see_parser_t* acp)
{
  int c;
  return SIMPLE_RULE(acp,((c=NEXT_CHR(acp))=='e' || c =='E') &&
      OPTIONAL(acp,(c=NEXT_CHR(acp))=='+' || c=='-',,) && ONE_OR_MORE(acp,RANGE(acp,"0-9"),,));
}

int a_see_parser_floating_point(a_see_parser_t*acp)
{
  return CAPTURE_ON(acp) &&
      (SIMPLE_RULE(acp,ONE_OR_MORE(acp,RANGE(acp,"0-9"),,) &&
        OPTIONAL(acp,NEXT_CHR(acp) == '.' && ZERO_OR_MORE(acp,RANGE(acp,"0-9"),,),,)
        && a_see_parser_exponent(acp))
      ||
      SIMPLE_RULE(acp,ZERO_OR_MORE(acp,RANGE(acp,"0-9"),,) &&
        NEXT_CHR(acp) == '.' && ONE_OR_MORE(acp,RANGE(acp,"0-9"),,)
        && OPTIONAL(acp,a_see_parser_exponent(acp),,))) && CAPTURE_OFF(acp) && a_see_parser_space(acp);
/*  return CAPTURE_ON(acp) && (SIMPLE_RULE(acp,
    a_see_parser_whole(acp) && OPTIONAL(acp,a_see_parser_fraction(acp),,) && a_see_parser_exponent(acp))
    ||
    SIMPLE_RULE(acp,OPTIONAL(acp,a_see_parser_whole(acp),,) && a_see_parser_fraction(acp)
        && OPTIONAL(acp,a_see_parser_exponent(acp),,))) && CAPTURE_OFF(acp);*/
}

int a_see_parser_decimal_integer(a_see_parser_t*acp)
{
  return CAPTURE_ON(acp) && SIMPLE_RULE(acp,RANGE(acp,"1-9") &&
      ZERO_OR_MORE(acp,RANGE(acp,"0-9"),,)) && CAPTURE_OFF(acp) && a_see_parser_space(acp);
}

int a_see_parser_octal_integer(a_see_parser_t*acp)
{
  return CAPTURE_ON(acp) && SIMPLE_RULE(acp,NEXT_CHR(acp)=='0' &&
      ZERO_OR_MORE(acp,RANGE(acp,"0-9"),,)) && CAPTURE_OFF(acp) && a_see_parser_space(acp);
}

int a_see_parser_hex_integer(a_see_parser_t*acp)
{
  int c;
  return CAPTURE_ON(acp) && SIMPLE_RULE(acp,NEXT_CHR(acp)=='0' && ((c=NEXT_CHR(acp)) == 'x' || c=='X')
        && ONE_OR_MORE(acp,RANGE(acp,"0-9a-fA-F"),,)) && CAPTURE_OFF(acp) && a_see_parser_space(acp);
}

int a_see_parser_ident(a_see_parser_t* acp)
{
  return CAPTURE_ON(acp) && SIMPLE_RULE(acp,RANGE(acp,"_a-zA-Z") &&
        ZERO_OR_MORE(acp,RANGE(acp,"_a-zA-Z0-9"),,)) && CAPTURE_OFF(acp) && a_see_parser_space(acp);
}
