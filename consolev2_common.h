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

#ifndef __CONSOLEV2_COMMON_H_
#define __CONSOLEV2_COMMON_H_

#include <SDL/SDL.h>
#include <SDL/SDL_terminal.h>

/*
 * CUSTOM TYPES
 */

typedef enum {
    AST_PLUS,
    AST_MINUS,
    AST_TIMES,
    AST_DIV,
    AST_MOD,
    AST_UNARY_MINUS,
    AST_BOOLEXPR,
    AST_STRING,
    AST_INTEGER,
    AST_FLOAT,
    AST_SYMREF,
    AST_IF,
    AST_WHILE,
    AST_FOR,
    AST_ASSIGN,
    AST_SPFUNC,
    AST_AND,
    AST_OR,
    AST_NOT,
    AST_TURTLE,
    AST_EXIT,
    AST_SHOWHELP,
    AST_ECHO,
    AST_LOADFILE,
    AST_STATEMENTS
} ast_type;

typedef enum {
    SPFUNC_COS,
    SPFUNC_SIN,
    SPFUNC_TAN,
    SPFUNC_ABS,
    SPFUNC_SQRT,
    SPFUNC_LOG,
    SPFUNC_LOG10,
    SPFUNC_EXP,
    SPFUNC_RMDR,
    SPFUNC_MAX,
    SPFUNC_MIN,
    SPFUNC_CEIL,
    SPFUNC_FLOOR
} spfunc_type;

typedef enum {
    OP_EQ,
    OP_NEQ,
    OP_LESS,
    OP_GREATER,
    OP_LEQ,
    OP_GEQ
} boolop_type;

typedef enum {
    TURT_FORWARD,
    TURT_BACKWARD,
    TURT_LEFT,
    TURT_RIGHT,
    TURT_PENDOWN,
    TURT_PENUP,
    TURT_HIDE,
    TURT_SHOW,
    TURT_WRITE,
    TURT_SET_COLOR, /* might go unused */
    TURT_CENTERED_CIRCLE,
    TURT_CIRCLE,
    TURT_HOME,
    TURT_CLEAR,
    TURT_RESET
} turt_action_type;

struct ast_node {
    ast_type type;
    union {
        struct {
            struct ast_node* left;
            struct ast_node* right;
        } expr;
        struct {
            struct ast_node* left;
            struct ast_node* right;
            boolop_type op;
        } boolexpr;
        struct {
            struct ast_node* condition;
            struct ast_node* ifactions;
            struct ast_node* elseactions;
        } ifexpr;
        struct {
            struct ast_node* condition;
            struct ast_node* loopactions;
        } whileexpr;
        struct {
            char* cursorname;
            struct ast_node* begin;
            struct ast_node* end;
            struct ast_node* loopactions;
        } forexpr;
        struct {
            char* name;
        } symrefexpr;
        struct {
            char* name;
            struct ast_node* val;
        } assignexpr;
        struct {
            struct ast_node* left;
            struct ast_node* right;
            spfunc_type type;
        } spfuncexpr;
        int intval;
        float fltval;
        char* strval;
        struct {
            turt_action_type type;
            struct ast_node* param;
        } turtleexpr;
    } data;
};

struct var_list {
    char* name;
    float val;
    struct var_list* next;
};

/*struct include_stack {
    FILE* handle;
};*/

struct exec_env {
    SDL_Terminal* term;
    SDL_Surface* screen;
    struct Turtle* turt;
    struct var_list** vars;
    int max_vars;
    bool shouldExit;
};

/*
 * UTILITY API
 */

void* malloc_or_die(size_t size);

/*
 * LOOKUP TABLE API
 */

unsigned int strhash(struct exec_env* env, char* str);

struct var_list* var_get(struct exec_env* env, char* name);

struct var_list* var_set(struct exec_env* env, char* name, float val);

/*
 * AST GENERATION API
 */

struct ast_node* ast_make(ast_type type, struct ast_node* left, struct ast_node* right);

struct ast_node* ast_make_boolexpr(boolop_type op, struct ast_node* left, struct ast_node* right);

struct ast_node* ast_make_if(struct ast_node* condition, struct ast_node* ifactions, struct ast_node* elseactions);

struct ast_node* ast_make_while(struct ast_node* condition, struct ast_node* loopactions);

struct ast_node* ast_make_for(char* cursorname, struct ast_node* begin, struct ast_node* end, struct ast_node* loopactions);

struct ast_node* ast_make_symref(char* name);

struct ast_node* ast_make_assign(char* name, struct ast_node* val);

struct ast_node* ast_make_spfunc(spfunc_type type, struct ast_node* left, struct ast_node* right);

struct ast_node* ast_make_turtle(turt_action_type type, struct ast_node* param);

struct ast_node* ast_make_integer(int intval);

struct ast_node* ast_make_float(float fltval);

struct ast_node* ast_make_string(char* strval);

/*
 * AST EXECUTION API
 */

void ast_run(struct exec_env* env, struct ast_node* ast);

bool ast_eval_boolexpr(struct exec_env* env, struct ast_node* ast);

int ast_eval_as_int(struct exec_env* env, struct ast_node* ast);

float ast_eval_as_float(struct exec_env* env, struct ast_node* ast);

char* ast_eval_as_string(struct exec_env* env, struct ast_node* ast);

/*
 * AST CLEANUP API
 */

void ast_destroy(struct ast_node* ast);

#endif /* __CONSOLEV2_COMMON_H_ */
