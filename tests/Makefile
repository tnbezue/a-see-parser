CC ?= gcc
LD = ${CC}
TARGETS= test_a_see_parser
CFLAGS=-g -I../include -Wall
LDFLAGS=-g
TEST_A_SEE_PARSER_OBJECTS=test_a_see_parser.o ../src/a_see_parser.o

all: ${TARGETS}

test_a_see_parser: ${TEST_A_SEE_PARSER_OBJECTS}
	${LD} ${LDFLAGS} ${TEST_A_SEE_PARSER_OBJECTS} -o $@

clean:
	rm -f ${TARGETS} *.o

test_a_see_parser.o: test_a_see_parser.c test_harness.h \
 ../include/a_see_parser/a_see_parser.h
