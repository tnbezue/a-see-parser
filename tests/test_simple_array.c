#include <string.h>
#include "test_harness.h"
#include <a_see_parser/simple_array.h>

void test_init() { }

void test_new_delete()
{
  TESTCASE("New/Delete");
  simple_array_t* sa = new_simple_array(sizeof(int),NULL,NULL,NULL,NULL);
  printf("sa pointer is %p\n",sa);
  TEST("Size is 0",simple_array_size(sa)==0);
  TEST("Max size is 0",simple_array_capacity(sa) == 0);
  TEST("Data is NULL",simple_array_begin(sa) == NULL);
  delete_simple_array(sa);
}

void test_push_back()
{
  TESTCASE("Push back");
  char msg[128];
  simple_array_t* sa = new_simple_array(sizeof(int),NULL,NULL,NULL,NULL);
  int test_data[] = { 987, 234, 435, 984, 231, -22, -98 , 0, 78232534, 123456789 };
  int i;
  int n = ARRAY_SIZE(test_data);
  for(i=0;i<n;i++)
    simple_array_push_back(sa,test_data+i);
  sprintf(msg,"Size after adding %d integers",n);
  TEST(msg,simple_array_size(sa) == n);
  for(i=0;i<n;i++) {
    sprintf(msg,"a[%d]=%d",i,test_data[i]);
    TEST(msg,*(int*)simple_array_at(sa,i) == test_data[i]);
  }
  delete_simple_array(sa);
}

void test_pop_back()
{
  TESTCASE("Pop back");
  char msg[128];
  simple_array_t* sa = new_simple_array(sizeof(int),NULL,NULL,NULL,NULL);
  int test_data[] = { 987, 234, 435, 984, 231, -22, -98 , 0, 78232534, 123456789 };
  int i;
  int n = ARRAY_SIZE(test_data);
  for(i=0;i<n;i++)
    simple_array_push_back(sa,test_data+i);
  for(i=0;i<n/2;i++)
    simple_array_pop_back(sa);
  sprintf(msg,"Size after adding %d integers and removing %d",n,n/2);
  TEST(msg,simple_array_size(sa) == n/2);
  for(i=0;i<n/2;i++) {
    sprintf(msg,"a[%d]=%d",i,test_data[i]);
    TEST(msg,*(int*)simple_array_at(sa,i) == test_data[i]);
  }
  delete_simple_array(sa);
}

int cmp_int(const void* lhs,const void* rhs)
{
  return (*(const int*)lhs > *(const int*)rhs) - (*(const int*)lhs < *(const int*)rhs);
}

void test_simple_array_find()
{
  TESTCASE("Integer array find");
  char msg[128];
  simple_array_t* sa = new_simple_array(sizeof(int),NULL,NULL,NULL,cmp_int);
  int test_data[] = { 987, 234, 435, 984, 231, -22, -98 , 0, 78232534, 123456789 ,
            987, 234, 435, 984, 231, -22, -98 , 0, 78232534, 123456789 ,
            987, 234, 435, 984, 231, -22, -98 , 0, 78232534, 123456789};
  int i;
  int n = ARRAY_SIZE(test_data);
  for(i=0;i<n;i++)
    simple_array_push_back(sa,test_data+i);
  sprintf(msg,"Size after adding %d integers",n);
  TEST(msg,simple_array_size(sa) == n);
  int cap = (n/SIMPLE_ARRAY_INITIAL_CAPACITY + 1)*SIMPLE_ARRAY_INITIAL_CAPACITY;
  sprintf(msg,"Capacity is %d",cap);
  TEST(msg,simple_array_capacity(sa)==cap);
  int* iptr = simple_array_find(sa,test_data+8,0);
  sprintf(msg,"Finding %d in array found at offset of 8",test_data[8]);
  TEST(msg,*iptr == test_data[8] && (iptr - ((int*)simple_array_begin(sa))) == 8);
  iptr = simple_array_find(sa,test_data+15,10);
  sprintf(msg,"Finding %d in array starting at offset of %d found at offset of 15",test_data[15],10);
  TEST(msg,*iptr == test_data[15] && (iptr - ((int*)simple_array_begin(sa))) == 15);
  iptr = simple_array_find(sa,test_data+27,20);
  sprintf(msg,"Finding %d in array starting at offset of %d found at offset of 27",test_data[27],20);
  TEST(msg,*iptr == test_data[27] && (iptr - ((int*)simple_array_begin(sa))) == 27);
  iptr = simple_array_find(sa,&n,0);
  sprintf(msg,"Finding %d not found in array",n);
  TEST(msg,iptr == simple_array_end(sa));
  delete_simple_array(sa);
}

typedef struct {
  char* name;
  int nshows;
} my_type_t;

typedef my_type_t* my_type_iterator;
typedef const my_type_t* my_type_const_iterator;

void my_type_default_ctor(void *mt)
{
  my_type_t* pmt = mt;
  pmt->name=NULL;
  pmt->nshows = 0;
}

void my_type_dtor(void *mt)
{
  free(((my_type_t*)mt)->name);
};

void my_type_copy(void *to,const void* from)
{
  ((my_type_t*)to)->name = strdup(((const my_type_t*)from)->name);
  ((my_type_t*)to)->nshows = ((const my_type_t*)from)->nshows;
}

int cmp_my_type(const void* lhs,const void* rhs)
{
  return strcmp(((const my_type_t*)lhs)->name,((const my_type_t*)rhs)->name);
}

void test_custom_type()
{
  TESTCASE("Custom type array");
  char msg[128];
  my_type_t test_data [] = {
    { "Elmer Fudd", 1234},
    { "Tom & Jerry", 1428},
    { "Bugs Bunny", 2000 },
    { "Road Runner", 384},
    { "Tweety Bird", 1234},
    { "Daffy Duck", 1992},
  };

  simple_array_t* my_sa = new_simple_array(sizeof(my_type_t),
          my_type_default_ctor,my_type_dtor,my_type_copy,cmp_my_type);
  int i,n=ARRAY_SIZE(test_data);
  for(i=0;i<n;i++)
    simple_array_push_back(my_sa,test_data+i);
  sprintf(msg,"Size after adding %d custom types",n);
  TEST(msg,simple_array_size(my_sa) == n);
  my_type_const_iterator imt = simple_array_cbegin(my_sa);
  for(i=0;i<n;i++,imt++) {
    sprintf(msg,"my_sa[%d] = { \"%s\",%d }",i,test_data[i].name,test_data[i].nshows);
    TEST(msg,cmp_my_type(test_data+i,imt)==0);
  }
  my_type_t val = { "Bugs Bunny", 0 };
  imt = simple_array_find(my_sa,&val,0);
  TEST("Found \"Bugs Bunny\" at 2",(imt - (my_type_t*)simple_array_begin(my_sa))==2);
  simple_array_pop_back(my_sa);
  simple_array_pop_back(my_sa);
  sprintf(msg,"Size after removing 2 custom types");
  TEST(msg,simple_array_size(my_sa) == n-2);
  imt = simple_array_cbegin(my_sa);
  for(i=0;i<n-2;i++,imt++) {
    sprintf(msg,"my_sa[%d] = { \"%s\",%d }",i,test_data[i].name,test_data[i].nshows);
    TEST(msg,cmp_my_type(test_data+i,imt)==0);
  }
  simple_array_pop_back(my_sa);
  simple_array_pop_back(my_sa);
  delete_simple_array(my_sa);
}


test_function tests[] =
{
  test_new_delete,
  test_push_back,
  test_pop_back,
  test_simple_array_find,
  test_custom_type,
};

TEST_MAIN(tests)
