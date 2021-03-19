CC ?= gcc
LD = ${CC}
TARGETS=demo_a_see_parser
CFLAGS=-g -I../../include -Wall
LDFLAGS=-g
DEMO_A_SEE_PARSER_OBJECTS=demo_a_see_parser.o ../../src/a_see_parser.o

all: ${TARGETS}

demo_a_see_parser: ${DEMO_A_SEE_PARSER_OBJECTS}
	${LD} ${LDFLAGS} ${DEMO_A_SEE_PARSER_OBJECTS} -o $@

clean:
	rm -f ${TARGETS} *.o

demo_a_see_parser.o: demo_a_see_parser.c ../../include/a_see_parser/a_see_parser.h
