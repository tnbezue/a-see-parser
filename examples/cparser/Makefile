CC ?= gcc
LD = ${CC}
TARGETS=cparser
CFLAGS+=-g -I../../include -Wall
LDFLAGS+=-g
CPARSER_OBJECTS=cparser.o ../../src/a_see_parser.o

all: ${TARGETS}

cparser: ${CPARSER_OBJECTS}
	${LD} ${LDFLAGS} ${CPARSER_OBJECTS} -lm -o $@

clean:
	rm -f ${TARGETS} *.o

cparser.o: cparser.c ../../include/a_see_parser/a_see_parser.h
