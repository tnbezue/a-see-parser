CC ?= gcc
LD = ${CC}
TARGETS=calculator_ast
CFLAGS=-g -I../../include -Wall -fms-extensions
LDFLAGS=-g
CALCULATOR_AST_OBJECTS=calculator_ast.o ast_tree.o ../../src/a_see_parser.o

all: ${TARGETS}

${TARGETS}: ${CALCULATOR_AST_OBJECTS}
	${LD} ${LDFLAGS} ${CALCULATOR_AST_OBJECTS} -lm -o $@

clean:
	rm -f ${TARGETS} *.o

calculator.o: calculator.c ../../include/a_see_parser/a_see_parser.h
ast_tree.o: ast_tree.c ast_tree.h
calculator_ast.o: calculator_ast.c ../../include/a_see_parser/a_see_parser.h ast_tree.h
