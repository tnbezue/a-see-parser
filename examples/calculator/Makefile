CC ?= gcc
LD = ${CC}
TARGETS=calculator
CFLAGS=-g -I../../include -Wall
LDFLAGS=-g
CALCULATOR_OBJECTS=calculator.o ../../src/a_see_parser.o

all: ${TARGETS}

calculator: ${CALCULATOR_OBJECTS}
	${LD} ${LDFLAGS} ${CALCULATOR_OBJECTS} -lm -o $@

clean:
	rm -f ${TARGETS} *.o

calculator.o: calculator.c ../../include/a_see_parser/a_see_parser.h
