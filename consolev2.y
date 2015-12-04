/*
 * Copyright 2015 Mathias Leyendecker / University of Strasbourg
 *
 * This file is part of MTurtle.
 * MTurtle is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * MTurtle is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MTurtle.  If not, see <http://www.gnu.org/licenses/>.
 *
 * ====================================================================
 *
 * MTurtle Console
 * Command line interpreter for Turtle graphics
 */
 
 /*%error-verbose*/
%define parse.error verbose
%code requires {
    #include <stdio.h>
    #include <stdlib.h>
    #include <stdbool.h>
    #include <string.h>
    /*#include <unistd.h>*/
    #include <errno.h>
    #include <SDL/SDL.h>
    #include <SDL/SDL_image.h>
    #include <SDL/SDL_ttf.h>
    /*#include <SDL/SDL_terminal.h>*/ /* not protected against double inclusion */
    #include "MTurtle.h"
    #include "consolev2_common.h"
    
    /*#define YYPARSE_PARAM ast_result*/
    
    /* Declarations from Lex */
    void yyerror(struct ast_node** ast_result, const char* msg);
    struct exec_env* yyextra;
    extern FILE* yyin;
    extern void scan_string(char* s);
    extern void clean_buffer();
    /*typedef struct yy_buffer_state * YY_BUFFER_STATE;
    extern int yyparse();
    extern YY_BUFFER_STATE yy_scan_string(char * str);
    extern void yy_delete_buffer(YY_BUFFER_STATE buffer);*/
}

%union {
    char* name;
    int intval;
    float fltval;
    char* strval;
    struct ast_node* ast;
    boolop_type boolop;
}

%parse-param {struct ast_node** ast_result}

%token TK_EXIT TK_HELP TK_FORWARD TK_BACKWARD TK_LEFT TK_RIGHT TK_PENDOWN TK_PENUP
%token TK_CIRCLE TK_CENTEREDCIRCLE TK_WRITE TK_HOME TK_CLEAR TK_RESET TK_ECHO
%token TK_LOAD TK_IF TK_THEN TK_ELSE TK_WHILE TK_FOR TK_FROM TK_TO TK_DO
%token TK_AND TK_OR TK_NOT TK_ENDIF TK_ENDFOR TK_ENDWHILE TK_EQ TK_NEQ TK_GEQ TK_LEQ
%token TK_ASSIGN TK_NEWLINE TK_NOELSE TK_EOF TK_HIDETURTLE TK_SHOWTURTLE
%token TK_COS TK_SIN TK_TAN TK_ABS TK_SQRT TK_LOG TK_LOG10 TK_EXP TK_RMDR
%token TK_MAX TK_MIN TK_CEIL TK_FLOOR TK_REPEAT TK_TIMES TK_ENDREPEAT
%token TK_FUNC TK_ENDFUNC TK_RETURN
%token <name> TK_IDENTIFIER
%token <intval> TK_INTEGER
%token <fltval> TK_FLOAT
%token <strval> TK_STRING

%nonassoc TK_NOELSE
%nonassoc TK_ELSE
%left '+' '-'
%left '*' '/' '%' TK_RMDR
%left TK_AND TK_OR
%right TK_ASSIGN
%left '(' ')'

%type <ast> statements statement assignment expression optional_expr boolexpr turt_forward turt_backward turt_left turt_right
%type <ast> turt_circle turt_centered_circle turt_write echo load_file blc_if blc_while blc_for blc_repeat
%type <ast> printable number loop_value basic_func blc_func expr_list idf_list
%type <boolop> boolop

%start top_level


%%

top_level
    : statements { *ast_result = $1; }
    | statement { *ast_result = $1; }
    | newlines { *ast_result = NULL; }
    | %empty { *ast_result = NULL; }
;

statements
    : statements statement newlines { $$ = ast_make(AST_STATEMENTS, $1, $2); }
    | statement newlines { $$ = $1; }
;

statement
    : TK_EXIT { $$ = ast_make(AST_EXIT, NULL, NULL); }
    | TK_HELP { $$ = ast_make(AST_SHOWHELP, NULL, NULL); }
    | turt_forward { $$ = $1; }
    | turt_backward { $$ = $1; }
    | turt_left { $$ = $1; }
    | turt_right { $$ = $1; }
    | TK_PENDOWN { $$ = ast_make_turtle(TURT_PENDOWN, NULL); }
    | TK_PENUP { $$ = ast_make_turtle(TURT_PENUP, NULL); }
    | TK_HIDETURTLE { $$ = ast_make_turtle(TURT_HIDE, NULL); }
    | TK_SHOWTURTLE { $$ = ast_make_turtle(TURT_SHOW, NULL); }
    | turt_circle { $$ = $1; }
    | turt_centered_circle { $$ = $1; }
    | turt_write { $$ = $1; }
    | TK_HOME { $$ = ast_make_turtle(TURT_HOME, NULL); }
    | TK_CLEAR { $$ = ast_make_turtle(TURT_CLEAR, NULL); }
    | TK_RESET { $$ = ast_make_turtle(TURT_RESET, NULL); }
    | echo { $$ = $1; }
    | load_file { $$ = $1; }
    | blc_if { $$ = $1; }
    | blc_while { $$ = $1; }
    | blc_for { $$ = $1; }
    | blc_repeat { $$ = $1; }
    | blc_func { $$ = $1; }
    | TK_RETURN { $$ = ast_make(AST_RETURN, NULL, NULL); }
    | TK_RETURN expression { $$ = ast_make(AST_RETURN, $2, NULL); }
    | assignment { $$ = $1; }
    | TK_IDENTIFIER '(' ')' { $$ = ast_make(AST_CALL, ast_make_string($1), NULL); }
    | TK_IDENTIFIER '(' expr_list ')' { $$ = ast_make(AST_CALL, ast_make_string($1), $3); }
    | TK_IDENTIFIER {
        char* str = malloc(1024);
        snprintf(str, 1024, "-!- Unknown command: %s", $1);
        $$ = ast_make(AST_ECHO, ast_make_string(str), NULL);
    }
;

assignment
    : TK_IDENTIFIER TK_ASSIGN expression { $$ = ast_make_assign($1, $3); }
    /*| assignment TK_ASSIGN expression*/
;

expression
    : TK_IDENTIFIER { $$ = ast_make_symref($1); }
    | TK_IDENTIFIER '(' ')' { $$ = ast_make(AST_CALL, ast_make_string($1), NULL); }
    | TK_IDENTIFIER '(' expr_list ')' { $$ = ast_make(AST_CALL, ast_make_string($1), $3); }
    | number { $$ = $1; }
    | '(' expression ')' { $$ = $2; }
    | expression '+' expression { $$ = ast_make(AST_PLUS, $1, $3); }
    | expression '-' expression { $$ = ast_make(AST_MINUS, $1, $3); }
    | expression '*' expression { $$ = ast_make(AST_TIMES, $1, $3); }
    | expression '/' expression { $$ = ast_make(AST_DIV, $1, $3); }
    | expression '%' expression { $$ = ast_make(AST_MOD, $1, $3); }
    /*| '-' expression %prec '-' { $$ = ast_make(AST_UNARY_MINUS, $2, NULL); }*/
    | '(' basic_func ')' { $$ = $2; }
    /*| basic_func { $$ = $1; }*/
    | expression TK_RMDR expression { $$ = ast_make_spfunc(SPFUNC_RMDR, $1, $3); }
;

optional_expr
    : expression { $$ = $1; }
    | %empty { $$ = ast_make_integer(0); }
;

expr_list
    : expression { $$ = ast_make(AST_EXPRS, $1, NULL); }
    | expression ',' expr_list { $$ = ast_make(AST_EXPRS, $1, $3); }
;

boolexpr
    : expression boolop expression { $$ = ast_make_boolexpr($2, $1, $3); }
    | '(' boolexpr ')' { $$ = $2; }
    | bool_not boolexpr { $$ = ast_make(AST_NOT, $2, NULL); }
    | '(' boolexpr TK_AND boolexpr ')'  { $$ = ast_make(AST_AND, $2, $4); }
    | '(' boolexpr TK_OR boolexpr ')'  { $$ = ast_make(AST_OR, $2, $4); }
    /*| boolexpr TK_AND boolexpr  { $$ = ast_make(AST_AND, $1, $3); }
    | boolexpr TK_OR boolexpr  { $$ = ast_make(AST_OR, $1, $3); }*/
;

boolop
    : TK_EQ { $$ = OP_EQ; }
    | TK_NEQ { $$ = OP_NEQ; }
    | '<' { $$ = OP_LESS; }
    | '>' { $$ = OP_GREATER; }
    | TK_LEQ { $$ = OP_LEQ; }
    | TK_GEQ { $$ = OP_GEQ; }
;

bool_not
    : '!'
    | TK_NOT
;

number
    : TK_INTEGER { $$ = ast_make_integer($1); }
    | TK_FLOAT { $$ = ast_make_float($1); }
;

turt_forward
    : TK_FORWARD optional_expr { $$ = ast_make_turtle(TURT_FORWARD, $2); }
;

turt_backward
    : TK_BACKWARD optional_expr  { $$ = ast_make_turtle(TURT_BACKWARD, $2); }
;

turt_left
    : TK_LEFT optional_expr { $$ = ast_make_turtle(TURT_LEFT, $2); }
;

turt_right
    : TK_RIGHT optional_expr { $$ = ast_make_turtle(TURT_RIGHT, $2); }
;

turt_circle
    : TK_CIRCLE optional_expr { $$ = ast_make_turtle(TURT_CIRCLE, $2); }
;

turt_centered_circle
    : TK_CENTEREDCIRCLE optional_expr { $$ = ast_make_turtle(TURT_CENTERED_CIRCLE, $2); }
;

printable
    : TK_STRING { $$ = ast_make_string($1); }
    | expression { $$ = $1; }
    | boolexpr { $$ = $1; }
;

turt_write
    : TK_WRITE printable { $$ = ast_make_turtle(TURT_WRITE, $2); }
;

echo
    : TK_ECHO printable { $$ = ast_make(AST_ECHO, $2, NULL); }
;

load_file
    : TK_LOAD TK_STRING { $$ = ast_make(AST_LOADFILE, ast_make_string($2), NULL); }
;

blc_if
    : TK_IF boolexpr TK_THEN optional_newlines statements TK_ENDIF %prec TK_NOELSE {
        $$ = ast_make_if($2, $5, NULL);
    }
    | TK_IF boolexpr TK_THEN optional_newlines statements TK_ELSE optional_newlines statements TK_ENDIF {
        $$ = ast_make_if($2, $5, $8);
    }
;

blc_while
    : TK_WHILE boolexpr TK_DO optional_newlines statements TK_ENDWHILE { $$ = ast_make_while($2, $5); }
;

loop_value
    : TK_IDENTIFIER { $$ = ast_make_symref($1); }
    | TK_INTEGER { $$ = ast_make_integer($1); }
;

blc_for
    : TK_FOR TK_IDENTIFIER TK_FROM loop_value TK_TO loop_value TK_DO optional_newlines statements TK_ENDFOR {
        $$ = ast_make_for($2, $4, $6, $9);
    }
;

blc_repeat
    : TK_REPEAT expression TK_TIMES optional_newlines statements TK_ENDREPEAT {
        $$ = ast_make_repeat($2, $5);
    }
;

blc_func
    : TK_FUNC TK_IDENTIFIER '(' idf_list ')' optional_newlines statements TK_ENDFUNC {
        $$ = ast_make_function($2, $4, $7);
    }
;

idf_list
    : TK_IDENTIFIER { $$ = ast_make(AST_PARAM, ast_make_string($1), NULL); }
    | TK_IDENTIFIER ',' idf_list { $$ = ast_make(AST_PARAM, ast_make_string($1), $3); }
    | %empty { $$ = NULL; }
;

basic_func
    : TK_COS expression { $$ = ast_make_spfunc(SPFUNC_COS, $2, NULL); }
    | TK_SIN expression { $$ = ast_make_spfunc(SPFUNC_SIN, $2, NULL); }
    | TK_TAN expression { $$ = ast_make_spfunc(SPFUNC_TAN, $2, NULL); }
    | TK_ABS expression { $$ = ast_make_spfunc(SPFUNC_ABS, $2, NULL); }
    | TK_SQRT expression { $$ = ast_make_spfunc(SPFUNC_SQRT, $2, NULL); }
    | TK_LOG expression { $$ = ast_make_spfunc(SPFUNC_LOG, $2, NULL); }
    | TK_LOG10 expression { $$ = ast_make_spfunc(SPFUNC_LOG10, $2, NULL); }
    | TK_EXP expression { $$ = ast_make_spfunc(SPFUNC_EXP, $2, NULL); }
    | TK_MAX expression expression { $$ = ast_make_spfunc(SPFUNC_MAX, $2, $3); }
    | TK_MIN expression expression { $$ = ast_make_spfunc(SPFUNC_MIN, $2, $3); }
    | TK_CEIL expression { $$ = ast_make_spfunc(SPFUNC_CEIL, $2, NULL); }
    | TK_FLOOR expression { $$ = ast_make_spfunc(SPFUNC_FLOOR, $2, NULL); }
;

newlines
    : newlines TK_NEWLINE
    | TK_NEWLINE
    | TK_EOF { /* might be unpractical */ }
;

optional_newlines
    : newlines
    | %empty
;

%%

#define TERMINAL_FONT_FILE "miscfixed.ttf"
#define TERMINAL_FONT_SIZE 12

SDL_Surface* screen;
struct Turtle* turt;
SDL_Terminal* term;

/*struct exec_env* env;*/

/*#define MAX_VARS 256
static struct var_list* vars[MAX_VARS];*/

/*static struct var_list* varlist = NULL;*/

void yyerror(struct ast_node** ast, const char* msg)
{
    SDL_TerminalPrint(term, "\033[31m%s\033[0m\n", msg);
}

int main(int argc, char** argv)
{
    /*printf("I am process %d\n", getpid());*/
    
    /* Init SDL */
    if(SDL_Init(SDL_INIT_VIDEO) == -1)
    {
        fprintf(stderr, "SDL_Init() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Init Screen Surface */
    screen = SDL_SetVideoMode(960, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if(screen == NULL)
    {
        fprintf(stderr, "SDL_SetVideoMode() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    /* Init SDL_ttf */
    if(TTF_Init() == -1)
    {
        fprintf(stderr, "TTF_Init() failed: %s\n", TTF_GetError());
        exit(EXIT_FAILURE);
    }

    /* Set Window Title */
    SDL_WM_SetCaption("Turtle Graphics Console", NULL);

    /* Init MTurtle */
    TT_InitMinimal(screen);
    turt = TT_Create(640, 480, 0, 0, 0);
    TT_SetSurfacePos(turt, 320, 0);
    TT_PenDown(turt);

    /* Init Terminal */
    term = SDL_CreateTerminal();
    if(SDL_TerminalSetFont(term, TERMINAL_FONT_FILE, TERMINAL_FONT_SIZE) == -1)
    {
        fprintf(stderr, "SDL_TerminalSetFont() failed: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
    SDL_TerminalSetSize(term, 80, 25);
    SDL_TerminalSetPosition(term, 10, 10);
    SDL_TerminalClear(term);

    SDL_TerminalPrint(term, "MTurtle Console\n\xA9 2015 Mathias Leyendecker\n>>> ");
    
    /* Init Environment Struct */
    struct exec_env env;
    env.term = term;
    env.screen = screen;
    env.turt = turt;
    /*env.vars = vars;
    env.max_vars = MAX_VARS;*/
    env.varlist = NULL;
    env.shouldExit = false;
    env.hasReturned = false;
    
    yyextra = &env;
    
    struct ast_node* ast = NULL;

    /* Main Loop */
    for(;;)
    {
        /* Check for User Events */
        SDL_Event ev;
        SDL_WaitEvent(&ev);

        /* User Exit */
        if(ev.type == SDL_QUIT)
        {
            break;
        }

        /* Process Terminal Events */
        if(ev.type == SDL_TERMINALEVENT)
        {
            /* Send Command to Lexer */
            scan_string(ev.user.data2);
            
            yyparse(&ast);
            clean_buffer();
            
            /* Run Generated AST */
            if(ast != NULL)
            {
                ast_run(&env, ast);
            }
            
            /* Exit as Necessary */
            if(env.shouldExit)
            {
                break;
            }
            
            /* Cleanup AST */
            ast_destroy(ast);
            ast = NULL;
            
            /* Cannot Return from Top Level */
            env.hasReturned = false;
            
            /* Print Prompt */
            SDL_TerminalPrint(term, ">>> ");
        }
        /*else if(ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE)
        {
            break;
        }*/

        /* Clear Screen */
        SDL_FillRect(screen, NULL, 0);

        /* Blit Turtle */
        TT_Blit(turt);

        /* Blit Terminal */
        SDL_TerminalBlit(term);

        /* Refresh Screen */
        SDL_Flip(screen);
    }
    
    TT_Destroy(turt);
    SDL_DestroyTerminal(term);
    TT_EndProgram();
    return EXIT_SUCCESS;
}