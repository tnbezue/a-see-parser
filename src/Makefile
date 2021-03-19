CC ?= gcc
LD = ${CC}
TARGETS=a_see_parser.o
CFLAGS=-g -I../include -Wall
LDFLAGS=-g

all: ${TARGETS}

clean:
	rm -f *.o

a_see_parser.o: a_see_parser.c ../include/a_see_parser/a_see_parser.h
