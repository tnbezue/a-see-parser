
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

/*
  The PEG grammar presented here is from
  https://github.com/pointlander/peg/blob/master/grammars/c/c.peg
*/

#include <stdio.h>
#include <stdlib.h>
#include <a_see_parser/a_see_parser.h>

DECLARE_DEFAULT_A_SEE_PARSER;
IMPLEMENT_DEFAULT_A_SEE_PARSER;

#define ID_CHAR RANGE("_a-zA-Z0-9")
#define ID_NONDIGIT RANGE("_a-zA-Z")

#define AUTO (SEQUENCE("auto") && !ID_CHAR && SPACE)
#define BREAK (SEQUENCE("break") && !ID_CHAR && SPACE)
#define CASE (SEQUENCE("case") && !ID_CHAR && SPACE)
#define CHAR (SEQUENCE("char") && !ID_CHAR && SPACE)
#define CONST (SEQUENCE("const") && !ID_CHAR && SPACE)
#define CONTINUE (SEQUENCE("continue") && !ID_CHAR && SPACE)
#define DEFAULT (SEQUENCE("default") && !ID_CHAR && SPACE)
#define DOUBLE (SEQUENCE("double") && !ID_CHAR && SPACE)
#define DO (SEQUENCE("do") && !ID_CHAR && SPACE)
#define ELSE (SEQUENCE("else") && !ID_CHAR && SPACE)
#define ENUM (SEQUENCE("enum") && !ID_CHAR && SPACE)
#define EXTERN (SEQUENCE("extern") && !ID_CHAR && SPACE)
#define FLOAT (SEQUENCE("float") && !ID_CHAR && SPACE)
#define FOR (SEQUENCE("for") && !ID_CHAR && SPACE)
#define GOTO (SEQUENCE("goto") && !ID_CHAR && SPACE)
#define IF (SEQUENCE("if") && !ID_CHAR && SPACE)
#define INT (SEQUENCE("int") && !ID_CHAR && SPACE)
#define INLINE (SEQUENCE("inline") && !ID_CHAR && SPACE)
#define LONG (SEQUENCE("long") && !ID_CHAR && SPACE)
#define REGISTER (SEQUENCE("register") && !ID_CHAR && SPACE)
#define RESTRICT (SEQUENCE("restrict") && !ID_CHAR && SPACE)
#define RETURN (SEQUENCE("return") && !ID_CHAR && SPACE)
#define SHORT (SEQUENCE("short") && !ID_CHAR && SPACE)
#define SIGNED (SEQUENCE("signed") && !ID_CHAR && SPACE)
#define SIZEOF (SEQUENCE("sizeof") && !ID_CHAR && SPACE)
#define STATIC (SEQUENCE("static") && !ID_CHAR && SPACE)
#define STRUCT (SEQUENCE("struct") && !ID_CHAR && SPACE)
#define SWITCH (SEQUENCE("switch") && !ID_CHAR && SPACE)
#define TYPEDEF (SEQUENCE("typedef") && !ID_CHAR && SPACE)
#define UNION (SEQUENCE("union") && !ID_CHAR && SPACE)
#define UNSIGNED (SEQUENCE("unsigned") && !ID_CHAR && SPACE)
#define VOID (SEQUENCE("void") && !ID_CHAR && SPACE)
#define VOLATILE (SEQUENCE("volatile") && !ID_CHAR && SPACE)
#define WHILE (SEQUENCE("while") && !ID_CHAR && SPACE)
#define BOOL (SEQUENCE("_Bool") && !ID_CHAR && SPACE)
#define COMPLEX (SEQUENCE("_Complex") && !ID_CHAR && SPACE)
#define IMAGINARY (SEQUENCE("_Imaginary") && !ID_CHAR && SPACE)
#define STDCALL (SEQUENCE("_stdcall") && !ID_CHAR && SPACE)
#define DECLSPEC (SEQUENCE("__declspec") && !ID_CHAR && SPACE)
#define ATTRIBUTE (SEQUENCE("__attribute__") && !ID_CHAR && SPACE)

#define ELLIPSIS (SEQUENCE("...") && SPACE)
#define LEFTEQU (SEQUENCE("<<=") && SPACE)
#define RIGHTEQU (SEQUENCE(">>=") && SPACE)
#define PTR (SEQUENCE("->") && SPACE)
#define INC (SEQUENCE("++") && SPACE)
#define DEC (SEQUENCE("--") && SPACE)
#define LEFT (SEQUENCE("<<") && SPACE)
#define RIGHT (SEQUENCE(">>") && SPACE)
#define LE (SEQUENCE("<=") && SPACE)
#define GE (SEQUENCE(">=") && SPACE)
#define EQUEQU (SEQUENCE("==") && SPACE)
#define BANGEQU (SEQUENCE("!=") && SPACE)
#define ANDAND (SEQUENCE("&&") && SPACE)
#define OROR (SEQUENCE("||") && SPACE)
#define STAREQU (SEQUENCE("*=") && SPACE)
#define DIVEQU (SEQUENCE("/=") && SPACE)
#define MODEQU (SEQUENCE("%=") && SPACE)
#define PLUSEQU (SEQUENCE("+=") && SPACE)
#define MINUSEQU (SEQUENCE("-=") && SPACE)
#define ANDEQU (SEQUENCE("&=") && SPACE)
#define HATEQU (SEQUENCE("^=") && SPACE)
#define OREQU (SEQUENCE("|=") && SPACE)
#define LBRK (NEXT_CHR_IS('[') && SPACE)
#define RBRK (NEXT_CHR_IS(']') && SPACE)
#define LPAR (NEXT_CHR_IS('(') && SPACE)
#define RPAR (NEXT_CHR_IS(')') && SPACE)
#define LWING (NEXT_CHR_IS('{') && SPACE)
#define RWING (NEXT_CHR_IS('}') && SPACE)
#define DOT (NEXT_CHR_IS('.') && SPACE)
#define AND (NEXT_CHR_IS('&') && SPACE)
#define STAR (NEXT_CHR_IS('*') && SPACE)
#define PLUS (NEXT_CHR_IS('+') && SPACE)
#define MINUS (NEXT_CHR_IS('-') && SPACE)
#define TILDA (NEXT_CHR_IS('~') && SPACE)
#define BANG (NEXT_CHR_IS('!') && SPACE)
#define DIV (NEXT_CHR_IS('/') && SPACE)
#define MOD (NEXT_CHR_IS('%') && SPACE)
#define LT (NEXT_CHR_IS('<') && SPACE)
#define GT (NEXT_CHR_IS('>') && SPACE)
#define HAT (NEXT_CHR_IS('^') && SPACE)
#define OR (NEXT_CHR_IS('|') && SPACE)
#define QUERY (NEXT_CHR_IS('?') && SPACE)
#define COLON (NEXT_CHR_IS(':') && SPACE)
#define SEMI (NEXT_CHR_IS(';') && SPACE)
#define EQU (NEXT_CHR_IS('=') && SPACE)
#define COMMA (NEXT_CHR_IS(',') && SPACE)
#define EOT !ANY

#define HEX_DIGIT RANGE("0-9a-fA-F")
#define HEX_PREFIX (SEQUENCE("0x") || SEQUENCE("0X"))

int translation_unit();
int external_declaration();
int function_definition();
int declaration();
int declarator();
int declaration_list();
int compound_statement();
int declaration();
int declaration_specifiers();
int init_declarator_list();
int storage_class_specifier();
int type_qualifier();
int function_specifier();
int typedef_name();
int init_declarator();
int initializer();
int struct_or_union_specifier();
int enum_specifier();
int identifier();
int struct_declaration();
int specifier_qualifier_list();
int struct_declarator_list();
int struct_declarator();
int constant_expression();
int enumerator_list();
int enumerator();
int enumeration_constant();
int pointer();
int direct_declarator();
int assignment_expression();
int parameter_type_list();
int identifier_list();
int parameter_list();
int parameter_declaration();
int abstract_declarator();
int direct_abstract_declarator();
int initializer_list();
int designation();
int designator();
int labeled_statement();
int expression_statement();
int selection_statement();
int iteration_statement();
int jump_statement();
int expression();
int constant();
int string_literal();
int argument_expression_list();
int unary_operator();
int cast_expression();
int assignment_operator();
int type_specifier();
int keyword();
int float_constant();
int integer_constant();
int character_constant();
int float_constant();
int decimal_float_constant();
int hex_float_constant();
int fraction();
int hexFraction();
int exponent();
int binary_exponent();
#define FLOAT_SUFFIX RANGE("flFL")
int enumeration_constant();
int character_constant();
int Char();
int escape();
int simple_escape();
int octal_escape();
int hex_escape();
int hex_fraction();
int decimal_constant();
int hex_constant();
int octal_constant();
int integer_suffix();
/*
int ();
int ();
int ();
int ();
int ();
int ();*/

// TranslationUnit <- Spacing ( ExternalDeclaration / SEMI ) * EOT
int translation_unit()
{
  return SPACE && RULE(ZERO_OR_MORE(external_declaration() || SEMI,,),,) && EOT;
}

// ExternalDeclaration <- FunctionDefinition / Declaration
int external_declaration()
{
  return function_definition() || declaration();
}

// FunctionDefinition <- DeclarationSpecifiers Declarator DeclarationList? CompoundStatement
int function_definition()
{
  return declaration_specifiers() && declarator()
      && OPTIONAL(declaration_list(),,) && compound_statement();
}

// DeclarationList <- Declaration+
int declaration_list()
{
  return ONE_OR_MORE(declaration(),,);
}

// Declaration <- DeclarationSpecifiers InitDeclaratorList? SEMI
int declaration()
{

  return RULE(declaration_specifiers() && OPTIONAL(init_declarator_list(),,) && SEMI,,);
}

/*
DeclarationSpecifiers
   <- (( StorageClassSpecifier
       / TypeQualifier
       / FunctionSpecifier
       )*
       TypedefName
       ( StorageClassSpecifier
       / TypeQualifier
       / FunctionSpecifier
       )*
      )     #{DeclarationSpecifiers}
    / ( StorageClassSpecifier
      / TypeSpecifier
      / TypeQualifier
      / FunctionSpecifier
      )+    #{DeclarationSpecifiers}
*/
int declaration_specifiers()
{
  return RULE(
    ZERO_OR_MORE(storage_class_specifier() || type_qualifier() || function_specifier(),,)
    && typedef_name() &&
    ZERO_OR_MORE(storage_class_specifier() || type_qualifier() || function_specifier(),,)
  ,,)
  ||
  ONE_OR_MORE(
    storage_class_specifier() || type_specifier() || type_qualifier()
        || function_specifier()
  ,,);
}

//InitDeclaratorList <- InitDeclarator (COMMA InitDeclarator)*
int init_declarator_list()
{
  return init_declarator() && ZERO_OR_MORE(COMMA && init_declarator(),,);
}

// InitDeclarator <- Declarator (EQU Initializer)? #{}
int init_declarator()
{
  return declarator() && OPTIONAL(EQU && initializer(),,);
}

/*
StorageClassSpecifier
   <- TYPEDEF
    / EXTERN
    / STATIC
    / AUTO
    / REGISTER
    / ATTRIBUTE LPAR LPAR (!RPAR .)* RPAR RPAR
*/
int storage_class_specifier()
{
  return RULE( TYPEDEF  || EXTERN || STATIC || AUTO || REGISTER ||
    (ATTRIBUTE && LPAR && LPAR && ZERO_OR_MORE(!NEXT_CHR_IS(')') && ANY,,) && RPAR && RPAR),,);
}
/*
TypeSpecifier
   <- VOID
    / CHAR
    / SHORT
    / INT
    / LONG
    / FLOAT
    / DOUBLE
    / SIGNED
    / UNSIGNED
    / BOOL
    / COMPLEX
    / StructOrUnionSpecifier
    / EnumSpecifier
*/
int type_specifier()
{
  return RULE( VOID || CHAR || SHORT || INT || LONG || FLOAT
      || DOUBLE || SIGNED || UNSIGNED || BOOL || COMPLEX
      || struct_or_union_specifier() || enum_specifier(),,);
}
/*
StructOrUnion <- STRUCT / UNION

StructOrUnionSpecifier
   <- StructOrUnion
      ( Identifier? LWING StructDeclaration* RWING
      / Identifier
      )
*/
int struct_or_union_specifier()
{
  return RULE( ( STRUCT || UNION) &&
      (
        (
          RULE(OPTIONAL(identifier(),,) && LWING && ZERO_OR_MORE(struct_declaration(),,) && RWING,,)
        ||
          identifier()
        )
      ),,);
}

// StructDeclaration <- ( SpecifierQualifierList StructDeclaratorList? )? SEMI
int struct_declaration()
{
  return RULE(OPTIONAL(specifier_qualifier_list() && OPTIONAL(struct_declarator_list(),,),,)
      && SEMI,,);
}

/*
SpecifierQualifierList
   <- ( TypeQualifier*
        TypedefName
        TypeQualifier*
      )
    / ( TypeSpecifier
      / TypeQualifier
      )+
*/
int specifier_qualifier_list()
{
  return RULE((ZERO_OR_MORE(type_qualifier(),,) && typedef_name()
      && ZERO_OR_MORE(type_qualifier(),,)),,)
  ||
  RULE(ONE_OR_MORE(type_specifier() || type_qualifier(),,),,);
}

// StructDeclaratorList <- StructDeclarator (COMMA StructDeclarator)*
int struct_declarator_list()
{
  return struct_declarator() &&
      ZERO_OR_MORE(COMMA && struct_declarator(),,);
}

/*
StructDeclarator
   <- Declarator? COLON ConstantExpression
    / Declarator
*/
int struct_declarator()
{
  return RULE(OPTIONAL(declarator(),,) && COLON && constant_expression(),,)
  || declarator();
}

/*
EnumSpecifier
    <- ENUM
      ( Identifier? LWING EnumeratorList COMMA? RWING
      / Identifier
      )
*/
int enum_specifier()
{
  return SIMPLE_RULE(ENUM &&
          (RULE(OPTIONAL(identifier(),,) && LWING
      && enumerator_list() && OPTIONAL(COMMA,,) && RWING,,)
      || identifier()));
}

// EnumeratorList <- Enumerator (COMMA Enumerator)*
int enumerator_list()
{
  return enumerator() && ZERO_OR_MORE(COMMA && enumerator(),,);
}

// Enumerator <- EnumerationConstant (EQU ConstantExpression)?
int enumerator()
{
  return enumeration_constant() && OPTIONAL(EQU && constant_expression(),,);
}

/*
TypeQualifier
   <- CONST
    / RESTRICT
    / VOLATILE
    / DECLSPEC LPAR Identifier RPAR
*/
int type_qualifier()
{
  return SIMPLE_RULE(CONST || RESTRICT || VOLATILE
      || (DECLSPEC && LPAR && identifier() && RPAR));
}

// FunctionSpecifier <- INLINE / STDCALL
int function_specifier()
{
  return SIMPLE_RULE(INLINE || STDCALL);
}

// Declarator <- Pointer? DirectDeclarator
int declarator()
{
  return OPTIONAL(pointer(),,) && direct_declarator();
}

/*
DirectDeclarator
   <- ( Identifier
      / LPAR Declarator RPAR
      )
      ( LBRK TypeQualifier* AssignmentExpression? RBRK
      / LBRK STATIC TypeQualifier* AssignmentExpression RBRK
      / LBRK TypeQualifier+ STATIC AssignmentExpression RBRK
      / LBRK TypeQualifier* STAR RBRK
      / LPAR ParameterTypeList RPAR
      / LPAR IdentifierList? RPAR
      )* #{}
*/
//( LBRK TypeQualifier* AssignmentExpression? RBRK
int direct_declarator_1()
{
  return RULE(LBRK && ZERO_OR_MORE(type_qualifier(),,)
      && OPTIONAL(assignment_expression(),,) && RBRK,,);
}

// LBRK STATIC TypeQualifier* AssignmentExpression RBRK
int direct_declarator_2()
{
  return RULE(LBRK && STATIC && ZERO_OR_MORE(type_qualifier(),,)
      && assignment_expression() && RBRK,,);
}

// LBRK TypeQualifier+ STATIC AssignmentExpression RBRK
int direct_declarator_3()
{
  return RULE(LBRK && ONE_OR_MORE(type_qualifier(),,)
        && STATIC && assignment_expression() && RBRK,,);
}

// LBRK TypeQualifier* STAR RBRK
int direct_declarator_4()
{
  return RULE(LBRK && ZERO_OR_MORE(type_qualifier(),,) && STAR && RBRK,,);
}

// LPAR ParameterTypeList RPAR
int direct_declarator_5()
{
  return RULE(LPAR && parameter_type_list() && RPAR,,);
}

// LPAR IdentifierList? RPAR
int direct_declarator_6()
{
  return RULE(LPAR && OPTIONAL(identifier_list(),,) && RPAR,,);
}

int direct_declarator()
{
  return RULE((identifier() ||
    (LPAR && ACP_TRACE_RULE(fputs("Got Lpar",stderr);) && declarator() && RPAR))
  && ZERO_OR_MORE(direct_declarator_1() || direct_declarator_2()
     || direct_declarator_3() || direct_declarator_4()
     || direct_declarator_5() || direct_declarator_6(),,),,);
}

//Pointer <- ( STAR TypeQualifier* )+
int pointer()
{
  return ZERO_OR_MORE(STAR && ZERO_OR_MORE(type_qualifier(),,),,);
}

//ParameterTypeList <- ParameterList (COMMA ELLIPSIS)?
int parameter_type_list()
{
  return parameter_list() &&
      OPTIONAL(COMMA && ELLIPSIS,,);
}

// ParameterList <- ParameterDeclaration (COMMA ParameterDeclaration)*
int parameter_list()
{
  return parameter_declaration()
        && ZERO_OR_MORE(COMMA && parameter_declaration(),,);
}

/*
ParameterDeclaration
   <- DeclarationSpecifiers
      ( Declarator
      / AbstractDeclarator
      )?
*/
int parameter_declaration()
{
  return declaration_specifiers()
      && OPTIONAL(declarator() || abstract_declarator(),,);
}

// IdentifierList <- Identifier (COMMA Identifier)*
int identifier_list()
{
  return identifier()
      && ZERO_OR_MORE(COMMA && identifier(),,);
}

// TypeName <- SpecifierQualifierList AbstractDeclarator?
int type_name()
{
  return specifier_qualifier_list() && OPTIONAL(abstract_declarator(),,);
}

/*
AbstractDeclarator
   <- Pointer? DirectAbstractDeclarator
    / Pointer
*/
int abstract_declarator()
{
  return (OPTIONAL(pointer(),,) && direct_abstract_declarator())
    || pointer();
}

// LPAR AbstractDeclarator RPAR
int direct_abstract_declarator_1()
{
  return RULE(LPAR && abstract_declarator()&& RPAR,,);
}

// LBRK (AssignmentExpression / STAR)? RBRK
int direct_abstract_declarator_2()
{
  return RULE(LBRK && OPTIONAL(assignment_expression() || STAR,,) && RBRK,,);
}

// LPAR ParameterTypeList? RPAR
int direct_abstract_declarator_3()
{
  return RULE(LPAR && OPTIONAL(parameter_type_list(),,) && RPAR,,);
}

/*
  DirectAbstractDeclarator
    <- ( LPAR AbstractDeclarator RPAR
      / LBRK (AssignmentExpression / STAR)? RBRK
      / LPAR ParameterTypeList? RPAR
      )
      ( LBRK (AssignmentExpression / STAR)? RBRK
      / LPAR ParameterTypeList? RPAR
      )*
*/
int direct_abstract_declarator()
{
  return RULE(direct_abstract_declarator_1() || direct_abstract_declarator_2() ||
    direct_abstract_declarator_3(),,) &&
    RULE(direct_abstract_declarator_2() || direct_abstract_declarator_3(),,);
}

// TypedefName <-Identifier
int typedef_name()
{
  return identifier();
}

/*
  Initializer
   <- AssignmentExpression
    / LWING InitializerList COMMA? RWING
*/
int initializer()
{
  return assignment_expression() ||
    RULE(LWING && initializer_list() && OPTIONAL(COMMA,,) && RWING,,);
}

//InitializerList <- Designation? Initializer (COMMA Designation? Initializer)*
int initializer_list()
{
  return RULE(OPTIONAL(designation(),,) && initializer() &&
      ZERO_OR_MORE(COMMA && OPTIONAL(designation(),,) && initializer(),,),,);
}

// Designation <- Designator+ EQU
int designation()
{
  return RULE(ONE_OR_MORE(designator(),,) && EQU,,);
}

/*
Designator
   <- LBRK ConstantExpression RBRK
    / DOT Identifier
*/
int designator()
{
  return RULE(LBRK && constant_expression() && RBRK,,)
    || RULE(DOT && identifier(),,);
}

/*
Statement
   <- LabeledStatement
    / CompoundStatement
    / ExpressionStatement
    / SelectionStatement
    / IterationStatement
    / JumpStatement
*/
int statement()
{
  return labeled_statement() || compound_statement() || expression_statement() ||
        selection_statement() || iteration_statement() || jump_statement();
}

/*
LabeledStatement
   <- Identifier COLON Statement
    / CASE ConstantExpression COLON Statement
    / DEFAULT COLON Statement
*/
int labeled_statement()
{
  return RULE(identifier() && COLON && statement(),,)
  ||
  RULE(CASE && constant_expression() && COLON && statement(),,)
  ||
  RULE(DEFAULT && COLON && statement(),,);
}

// CompoundStatement <- LWING ( Declaration / Statement )* RWING
int compound_statement()
{
  return RULE(LWING && ZERO_OR_MORE(declaration() || statement(),,) && RWING,,);
}

// ExpressionStatement <- Expression? SEMI
int expression_statement()
{
  return RULE(OPTIONAL(expression(),,) && SEMI,,);
}

/*
SelectionStatement
   <- IF LPAR Expression RPAR Statement (ELSE Statement)?
    / SWITCH LPAR Expression RPAR Statement
*/
int selection_statement()
{
  return RULE(IF && LPAR && expression() && RPAR && statement()
      && OPTIONAL(ELSE && statement(),,),,)
    ||
      RULE(SWITCH && LPAR && expression() && RPAR && statement(),,);
}

// WHILE LPAR Expression RPAR Statement
int iteration_statement_1()
{
  return RULE(WHILE && LPAR && expression() && RPAR && statement(),,);
}

// DO Statement WHILE LPAR Expression RPAR SEMI
int iteration_statement_2()
{
  return RULE(DO && statement() && WHILE && LPAR && expression() && RPAR && SEMI,,);
}

// FOR LPAR Expression? SEMI Expression? SEMI Expression? RPAR Statement
int iteration_statement_3()
{
  return RULE(FOR && LPAR &&
        OPTIONAL(expression(),,) && SEMI &&
        OPTIONAL(expression(),,) && SEMI &&
        OPTIONAL(expression(),,) && RPAR && statement(),,);
}

// FOR LPAR Declaration Expression? SEMI Expression? RPAR Statement
int iteration_statement_4()
{
  return RULE(FOR && LPAR && declaration() &&
      OPTIONAL(expression(),,) && SEMI &&
      OPTIONAL(expression(),,) && RPAR && statement(),,);
}

/*
IterationStatement
   <- WHILE LPAR Expression RPAR Statement
    / DO Statement WHILE LPAR Expression RPAR SEMI
    / FOR LPAR Expression? SEMI Expression? SEMI Expression? RPAR Statement
    / FOR LPAR Declaration Expression? SEMI Expression? RPAR Statement
*/

int iteration_statement()
{
  return iteration_statement_1() || iteration_statement_2()
      || iteration_statement_3() || iteration_statement_4();
}

// GOTO Identifier SEMI
int goto_jump_statement()
{
  return RULE(GOTO && identifier() && SEMI,,);
}

//  CONTINUE SEMI
int continue_jump_statement()
{
  return RULE(CONTINUE && SEMI,,);
}

//  BREAK SEMI
int break_jump_statement()
{
  return RULE(BREAK && SEMI,,);
}

// RETURN Expression? SEMI
int return_jump_statement()
{
  return RULE(RETURN && OPTIONAL(expression(),,) && SEMI,,);
}

/*
JumpStatement
   <- GOTO Identifier SEMI
    / CONTINUE SEMI
    / BREAK SEMI
    / RETURN Expression? SEMI
*/

int jump_statement()
{
  return goto_jump_statement() || continue_jump_statement()
      || break_jump_statement() || return_jump_statement();
}

/*
PrimaryExpression
   <- StringLiteral
    / Constant
    / Identifier
    / LPAR Expression RPAR
*/
int primary_expression()
{
  return string_literal() || constant() || identifier() ||
    RULE(LPAR && expression() && RPAR,,);
}

/*
PostfixExpression
   <- ( PrimaryExpression
      / LPAR TypeName RPAR LWING InitializerList COMMA? RWING
      )
      ( LBRK Expression RBRK
      / LPAR ArgumentExpressionList? RPAR
      / DOT Identifier
      / PTR Identifier
      / INC
      / DEC
      )*
*/
int postfix_expression_1()
{
  return RULE(LPAR && type_name() && RPAR
    && LWING && initializer_list() &&
    OPTIONAL(COMMA,,) && RWING,,);
}

int postfix_expression_2()
{
  return RULE(LBRK && expression() && RBRK,,);
}

int postfix_expression_3()
{
  return RULE(LPAR && ZERO_OR_MORE(argument_expression_list(),,) && RPAR,,);
}

int postfix_expression_4()
{
  return RULE(DOT && identifier(),,);
}

int postfix_expression_5()
{
  return RULE(PTR && identifier(),,);
}

int postfix_expression_6()
{
  return RULE(INC || DEC,,);
}

int postfix_expression()
{
  return RULE((primary_expression() || postfix_expression_1())
  && ZERO_OR_MORE(postfix_expression_2() || postfix_expression_3() ||
        postfix_expression_3() || postfix_expression_5()
        || postfix_expression_6(),,),,);
}

int argument_expression_list()
{
  return assignment_expression()
      && ZERO_OR_MORE(COMMA && assignment_expression(),,);
}

/*
UnaryExpression
   <- PostfixExpression
    / INC UnaryExpression
    / DEC UnaryExpression
    / UnaryOperator CastExpression
    / SIZEOF (UnaryExpression / LPAR TypeName RPAR )
*/
int unary_expression()
{
  return postfix_expression()
    ||
    RULE(INC && unary_expression(),,)
    ||
    RULE(DEC && unary_expression(),,)
    ||
    RULE(unary_operator() && cast_expression(),,)
    ||
    RULE(SIZEOF && (unary_expression() ||
              RULE(LPAR && type_name() && RPAR,,)),,);
}

/*
UnaryOperator
   <- AND
    / STAR
    / PLUS
    / MINUS
    / TILDA
    / BANG
*/

int unary_operator()
{
  return RULE(AND || STAR || PLUS || MINUS || TILDA || BANG,,);
}


// CastExpression <- (LPAR TypeName RPAR CastExpression) / UnaryExpression
int cast_expression()
{
  return RULE(LPAR && type_name()
        && RPAR && cast_expression(),,)
    ||
      unary_expression();
}


// MultiplicativeExpression <- CastExpression ((STAR / DIV / MOD) CastExpression)*
int multiplicative_expression()
{
  return cast_expression() &&
      ZERO_OR_MORE((STAR || DIV || MOD) && cast_expression(),,);
}


//AdditiveExpression <- MultiplicativeExpression ((PLUS / MINUS) MultiplicativeExpression)*
int additive_expression()
{
  return multiplicative_expression() &&
      ZERO_OR_MORE((PLUS || MINUS) && multiplicative_expression(),,);
}

//ShiftExpression <- AdditiveExpression ((LEFT / RIGHT) AdditiveExpression)*
int shift_expression()
{
  return RULE(additive_expression() && ZERO_OR_MORE(( LEFT || RIGHT) && additive_expression(),,),,);
}

//RelationalExpression <- ShiftExpression ((LE / GE / LT / GT) ShiftExpression)*
int relational_expression()
{
  return RULE(shift_expression() && ZERO_OR_MORE((LE || GE || LT || GT) && shift_expression(),,),,);
}


//EqualityExpression <- RelationalExpression ((EQUEQU / BANGEQU) RelationalExpression)*
int equality_expression()
{
  return RULE(relational_expression() &&
        ZERO_OR_MORE((EQUEQU || BANGEQU) && relational_expression(),,),,);
}

//ANDExpression <- EqualityExpression (AND EqualityExpression)*
int and_expression()
{
  return RULE(equality_expression() && ZERO_OR_MORE(AND && equality_expression(),,),,);
}

//ExclusiveORExpression <- ANDExpression (HAT ANDExpression)*
int exclusive_or_expression()
{
  return RULE(and_expression() && ZERO_OR_MORE(HAT && and_expression(),,),,);
}

//InclusiveORExpression <- ExclusiveORExpression (OR ExclusiveORExpression)*
int inclusive_or_expression()
{
  return RULE(exclusive_or_expression() && ZERO_OR_MORE(OR && exclusive_or_expression(),,),,);
}

//LogicalANDExpression <- InclusiveORExpression (ANDAND InclusiveORExpression)*
int logical_and_expression()
{
  return RULE(inclusive_or_expression() &&
      ZERO_OR_MORE(ANDAND && inclusive_or_expression(),,),,);
}

//LogicalORExpression <- LogicalANDExpression (OROR LogicalANDExpression)*
int logical_or_expression()
{
  return RULE(logical_and_expression() &&
      ZERO_OR_MORE(OROR && logical_and_expression(),,),,);
}

//ConditionalExpression <- LogicalORExpression (QUERY Expression COLON LogicalORExpression)*
int conditional_expression()
{
  return RULE(logical_or_expression() &&
      ZERO_OR_MORE(QUERY && expression() && COLON && logical_or_expression(),,),,);
}

/*
AssignmentExpression
   <- UnaryExpression AssignmentOperator AssignmentExpression
    / ConditionalExpression
*/
int assignment_expression()
{
  return RULE(unary_expression() && assignment_operator() && assignment_expression(),,)
        || conditional_expression();
}

/*
AssignmentOperator
   <- EQU
    / STAREQU
    / DIVEQU
    / MODEQU
    / PLUSEQU
    / MINUSEQU
    / LEFTEQU
    / RIGHTEQU
    / ANDEQU
    / HATEQU
    / OREQU
*/

int assignment_operator()
{
  return RULE(EQU || STAREQU || DIVEQU || MODEQU || PLUSEQU || MINUSEQU
        || LEFTEQU || RIGHTEQU || ANDEQU || HATEQU || OREQU,,);
}

// Expression <- AssignmentExpression (COMMA AssignmentExpression)*
int expression()
{
  return assignment_expression() && ZERO_OR_MORE(COMMA && assignment_expression(),,);
}

// ConstantExpression <- ConditionalExpression
int constant_expression()
{
  return conditional_expression();
}

/*
//Expression <- AssignmentExpression (COMMA AssignmentExpression)*
int expression()
{
  return assignment_expression() &&
      ZERO_OR_MORE(COMMA) && assignment_expression(),,);
}
*/

//EnumerationConstant <- Identifier
int enumeration_constant()
{
  return RULE(identifier(),,);
}

// StringLiteral <- 'L'? (["] StringChar* ["] Spacing)+
int string_literal()
{
  return RULE(OPTIONAL(NEXT_CHR=='L',,) && ONE_OR_MORE(DOUBLE_QUOTED_STRING && SPACE,,),,);
}

// Identifier <- !Keyword IdNondigit IdChar* Spacing #{}
int identifier()
{
  return RULE(!keyword() && ID_NONDIGIT && ZERO_OR_MORE(ID_CHAR,,) && SPACE,,);
}

/*
Constant
   <- FloatConstant
    / IntegerConstant       # Note: can be a prefix of Float Constant!
    / EnumerationConstant
    / CharacterConstant
*/
int constant()
{
  return float_constant() || integer_constant() || enumeration_constant() || character_constant();
}

#define LSUFFIX (SEQUENCE("ll") || SEQUENCE("Ll") || SEQUENCE("lL"))

/*
IntegerConstant
   <- ( DecimalConstant
      / HexConstant
      / OctalConstant
      )
    IntegerSuffix? Spacing
*/
int integer_constant()
{
  return RULE((decimal_constant() || hex_constant() || octal_constant())
        && OPTIONAL(integer_suffix(),,) && SPACE,,);
}

//DecimalConstant <- [1-9][0-9]*
int decimal_constant()
{
  return RULE(RANGE("1-9") && ZERO_OR_MORE(RANGE("0-9"),,),,);
}

//OctalConstant   <- '0' [0-7]*
int octal_constant()
{
  return RULE(NEXT_CHR_IS('0') && ZERO_OR_MORE(RANGE("0-7"),,),,);
}

//HexConstant     <- HexPrefix HexDigit+
int hex_constant()
{
  return RULE(HEX_PREFIX && ONE_OR_MORE(HEX_DIGIT,,),,);
}

/*
IntegerSuffix
   <- [uU] Lsuffix?
    / Lsuffix [uU]?
*/
int integer_suffix()
{
  return RULE(RANGE("uU") && OPTIONAL(LSUFFIX,,),,)
    ||
      RULE(LSUFFIX && OPTIONAL(RANGE("uU"),,),,);
}

/*
FloatConstant
   <- ( DecimalFloatConstant
      / HexFloatConstant
      )
    FloatSuffix? Spacing
*/
int float_constant()
{
  return RULE((decimal_float_constant() || hex_float_constant())
      && OPTIONAL(FLOAT_SUFFIX,,),,) && SPACE;
}
/*
DecimalFloatConstant
   <- Fraction Exponent?
    / [0-9]+ Exponent
*/
int decimal_float_constant()
{
  return RULE((fraction() && OPTIONAL(exponent(),,))
    || (ONE_OR_MORE(RANGE("0-9"),,) && exponent()),,);
}

/*HexFloatConstant
   <- HexPrefix HexFraction BinaryExponent?
    / HexPrefix HexDigit+ BinaryExponent
*/
int hex_float_constant()
{
  return RULE(HEX_PREFIX && hex_fraction() && OPTIONAL(binary_exponent(),,),,)
    || RULE(HEX_PREFIX && ONE_OR_MORE(HEX_DIGIT,,) && binary_exponent(),,);
}
/*
Fraction
   <- [0-9]* '.' [0-9]+
    / [0-9]+ '.'
*/
int fraction()
{
  return RULE(ZERO_OR_MORE(RANGE("0-9"),,) &&  NEXT_CHR_IS('.') && ONE_OR_MORE(RANGE("0-9"),,),,)
  || RULE(ONE_OR_MORE(RANGE("0-9"),,) && NEXT_CHR_IS('.'),,);
}

/*
HexFraction
   <- HexDigit* '.' HexDigit+
    / HexDigit+ '.'
*/
int hex_fraction()
{
  return RULE(ZERO_OR_MORE(HEX_DIGIT,,) && NEXT_CHR_IS('.') && ONE_OR_MORE(HEX_DIGIT,,),,)
    ||
    RULE(ONE_OR_MORE(HEX_DIGIT,,) && NEXT_CHR_IS('.'),,);
}

//Exponent <- [eE][+\-]? [0-9]+
int exponent()
{
  return RULE(RANGE("eE") && OPTIONAL(RANGE("-+"),,) && ONE_OR_MORE(RANGE("0-9"),,),,);
}

//BinaryExponent <- [pP][+\-]? [0-9]+
int binary_exponent()
{
  return RULE(RANGE("pP") && OPTIONAL(RANGE("-+"),,) && ONE_OR_MORE(RANGE("0-9"),,),,);
}

//EnumerationConstant <- Identifier
int enumerator_constant()
{
  return identifier();
}

//CharacterConstant <- 'L'? ['] Char* ['] Spacing
int character_constant()
{
  return RULE(OPTIONAL(NEXT_CHR_IS('L'),,) && NEXT_CHR_IS('\'')
      && ZERO_OR_MORE(Char(),,) && NEXT_CHR_IS('\''),,);
}

//Char <- Escape / !['\n\\] .
int Char()
{
  return escape() || RULE(!RANGE("'\n\\") && ANY,,);
}

/*
Escape
   <- SimpleEscape
    / OctalEscape
    / HexEscape
    / UniversalCharacter
*/
int escape()
{
  return simple_escape() || octal_escape() || hex_escape(); // || universal_character();
}

// SimpleEscape <- '\\' ['\"?\\%abfnrtv]
int simple_escape()
{
  return RULE(NEXT_CHR_IS('\\') && RANGE("'\"?\\%abfnrtv"),,);
}

//OctalEscape  <- '\\' [0-7][0-7]?[0-7]?
int octal_escape()
{
  return RULE(NEXT_CHR_IS('\\') && RANGE("0-7") && OPTIONAL(RANGE("0-7"),,) && OPTIONAL(RANGE("0-7"),,),,);
}

//HexEscape    <- '\\x' HexDigit+
int hex_escape()
{
  return RULE(SEQUENCE("\\x") && ONE_OR_MORE(HEX_DIGIT,,),,);
}

int keyword() {
  return SIMPLE_RULE(
    (
      SEQUENCE("auto") ||
      SEQUENCE("break") ||
      SEQUENCE("case") ||
      SEQUENCE("char") ||
      SEQUENCE("const") ||
      SEQUENCE("continue") ||
      SEQUENCE("default") ||
      SEQUENCE("double") ||
      SEQUENCE("do") ||
      SEQUENCE("else") ||
      SEQUENCE("enum") ||
      SEQUENCE("extern") ||
      SEQUENCE("float") ||
      SEQUENCE("for") ||
      SEQUENCE("goto") ||
      SEQUENCE("if") ||
      SEQUENCE("int") ||
      SEQUENCE("inline") ||
      SEQUENCE("long") ||
      SEQUENCE("register") ||
      SEQUENCE("restrict") ||
      SEQUENCE("return") ||
      SEQUENCE("short") ||
      SEQUENCE("signed") ||
      SEQUENCE("sizeof") ||
      SEQUENCE("static") ||
      SEQUENCE("struct") ||
      SEQUENCE("switch") ||
      SEQUENCE("typedef") ||
      SEQUENCE("union") ||
      SEQUENCE("unsigned") ||
      SEQUENCE("void") ||
      SEQUENCE("volatile") ||
      SEQUENCE("while") ||
      SEQUENCE("_Bool") ||
      SEQUENCE("_Complex") ||
      SEQUENCE("_Imaginary") ||
      SEQUENCE("_stdcall") ||
      SEQUENCE("__declspec") ||
      SEQUENCE("__attribute__")
    ) && !ID_CHAR
  );
}


int main(int argc,char* argv[])
{
  SET_PARSE_STRING("int main(int argc,char* argv[]) { }");
  if(translation_unit())
    puts("Good");
  else {
    puts("BAD");
  }
}
