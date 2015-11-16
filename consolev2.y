%error-verbose
%code requires {
    #include <stdio.h>
    #include <stdlib.h>
    #include "consolev2_common.h"
    
    #define YYPARSE_PARAM ast_result
}

%union {
    char* name;
    int intval;
    float fltval;
    char* strval;
    struct ast_node* ast;
}

%token TK_EXIT TK_HELP TK_FORWARD TK_BACKWARD TK_LEFT TK_RIGHT TK_PENDOWN TK_PENUP
%token TK_CIRCLE TK_CENTEREDCIRCLE TK_WRITE TK_HOME TK_CLEAR TK_RESET TK_ECHO
%token TK_LOAD TK_IF TK_THEN TK_ELSE TK_WHILE TK_FOR TK_FROM TK_TO TK_DO
%token TK_AND TK_OR TK_NOT TK_ENDIF TK_ENDFOR TK_ENDWHILE TK_EQ TK_NEQ TK_GEQ TK_LEQ
%token TK_ASSIGN TK_NEWLINE TK_NOELSE
%token TK_COS TK_SIN TK_TAN TK_ABS TK_SQRT TK_LOG TK_LOG10 TK_EXP TK_RMDR
%token TK_MAX TK_MIN TK_CEIL TK_FLOOR
%token <name> TK_IDENTIFIER
%token <intval> TK_INTEGER
%token <fltval> TK_FLOAT
%token <strval> TK_STRING

%left '+' '-'
%left '*' '/' '%'
%left TK_AND TK_OR
%right TK_ASSIGN

%%

statements
    : statements statement TK_NEWLINE
    | statement TK_NEWLINE
;

statement
    : TK_EXIT
    | TK_HELP
    | turt_forward
    | turt_backward
    | turt_left
    | turt_right
    | TK_PENDOWN
    | TK_PENUP
    | turt_circle
    | turt_centered_circle
    | turt_write
    | TK_HOME
    | TK_CLEAR
    | TK_RESET
    | echo
    | load_file
    | blc_if
    | blc_while
    | blc_for
    | assignment
    | expression { /* implicit echo */ }
;

assignment
    : TK_IDENTIFIER TK_ASSIGN expression
    | assignment TK_ASSIGN expression
;

expression
    : TK_IDENTIFIER
    | number
    | '(' expression ')' 
    | expression '+' expression
    | expression '-' expression
    | expression '*' expression
    | expression '/' expression
    | expression '%' expression
    | '-' expression %prec '-'
    | basic_func
;

boolexpr
    : expression boolop expression
    | '(' boolexpr ')'
    | bool_not boolexpr
    | boolexpr TK_AND boolexpr
    | boolexpr TK_OR boolexpr
;

boolop
    : TK_EQ
    | TK_NEQ
    | '<'
    | '>'
    | TK_LEQ
    | TK_GEQ
;

bool_not
    : '!'
    | TK_NOT
;

number
    : TK_INTEGER
    | TK_FLOAT
;

turt_forward
    : TK_FORWARD expression
;

turt_backward
    : TK_BACKWARD expression
;

turt_left
    : TK_LEFT expression
;

turt_right
    : TK_RIGHT expression
;

turt_circle
    : TK_CIRCLE expression
;

turt_centered_circle
    : TK_CENTEREDCIRCLE expression
;

printable
    : TK_STRING
    | expression
    | boolexpr
;

turt_write
    : TK_WRITE printable
;

echo
    : TK_ECHO printable
;

load_file
    : TK_LOAD TK_STRING
;

blc_if
    : TK_IF boolexpr TK_THEN statements TK_ENDIF %prec TK_NOELSE
    | TK_IF boolexpr TK_THEN statements TK_ELSE statements TK_ENDIF
;

blc_while
    : TK_WHILE boolexpr TK_DO statements TK_ENDWHILE
;

loop_value
    : TK_IDENTIFIER
    | TK_INTEGER
;

blc_for
    : TK_FOR TK_IDENTIFIER TK_FROM loop_value TK_TO loop_value TK_DO statements TK_ENDFOR
;

basic_func
    : TK_COS expression
    | TK_SIN expression
    | TK_TAN expression
    | TK_ABS expression
    | TK_SQRT expression
    | TK_LOG expression
    | TK_LOG10 expression
    | TK_EXP expression
    | expression TK_RMDR expression
    | TK_MAX expression expression
    | TK_MIN expression expression
    | TK_CEIL expression expression
;

%%

