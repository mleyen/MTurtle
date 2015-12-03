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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "MTurtle.h"
#include "consolev2_common.h"

#define PI 3.14159265
#define RAD2DEG PI / 180.0

extern struct ast_node* scan_file(char*);

/*
 * UTILITY API
 */

void* malloc_or_die(size_t size)
{
    void* ptr = malloc(size);
    if(ptr == NULL)
    {
        fprintf(stderr, "*** FATAL: malloc failed!\n");
        exit(EXIT_FAILURE);
    }
    
    return ptr;
}

/*
 * LOOKUP TABLE API
 */

unsigned int strhash(struct exec_env* env, char* str)
{
    unsigned int hashval;
    for(hashval = 0; *str != '\0'; str++)
    {
        hashval = *str + 31 * hashval;
    }
    return hashval % env->max_vars;
}

struct var_list* var_get(struct exec_env* env, char* name)
{
    struct var_list* cursor = env->vars[strhash(env, name)];
    while(cursor != NULL)
    {
        if(strcmp(name, cursor->name) == 0)
        {
            /*printf("Retrieving value for %s: %f\n", cursor->name, cursor->val);*/
            return cursor;
        }
        
        cursor = cursor->next;
    }
    
    return NULL;
}

struct var_list* var_set(struct exec_env* env, char* name, float val)
{
    struct var_list* cursor;
    unsigned int hashval;
    
    if(NULL == (cursor = var_get(env, name)))
    {
        /* New Variable */
        cursor = malloc_or_die(sizeof(struct var_list));
        if(cursor == NULL || NULL == (cursor->name = strdup(name)))
        {
            return NULL;
        }
        hashval = strhash(env, name);
        cursor->next = env->vars[hashval];
        env->vars[hashval] = cursor;
    }
    else
    {
        /* Already Defined Variable */
        if(cursor->isFunc == true)
        {
            printf("-!- Cannot override function %s!\n", name);
            return cursor;
        }
    }
    
    cursor->isFunc = false;
    cursor->val = val;
    printf("%s is set to %f\n", cursor->name, cursor->val);
    return cursor;
}

struct var_list* func_set(struct exec_env* env, char* name, int arg_count, char** arg_vector, struct ast_node* body)
{
    struct var_list* cursor;
    unsigned int hashval;
    
    if(NULL == (cursor = var_get(env, name)))
    {
        /* New Variable */
        cursor = malloc_or_die(sizeof(struct var_list));
        if(cursor == NULL || NULL == (cursor->name = strdup(name)))
        {
            return NULL;
        }
        hashval = strhash(env, name);
        cursor->next = env->vars[hashval];
        env->vars[hashval] = cursor;
    }
    else
    {
        /* Already Defined Variable */
        if(cursor->isFunc == true)
        {
            printf("-!- Cannot override function %s!\n", name);
            return cursor;
        }
    }
    
    cursor->isFunc = true;
    cursor->func.argc = arg_count;
    cursor->func.argv = arg_vector;
    cursor->func.body = body;
    printf("%s is defined\n", cursor->name);
    return cursor;
}

/*
 * AST GENERATION API
 */

struct ast_node* ast_make(ast_type type, struct ast_node* left, struct ast_node* right)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = type;
    ast->data.expr.left = left;
    ast->data.expr.right = right;
    
    /* should be enough */
    return ast;
}

struct ast_node* ast_make_boolexpr(boolop_type op, struct ast_node* left, struct ast_node* right)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_BOOLEXPR;
    ast->data.boolexpr.left = left;
    ast->data.boolexpr.right = right;
    ast->data.boolexpr.op = op;
    
    return ast;
}

struct ast_node* ast_make_if(struct ast_node* condition, struct ast_node* ifactions, struct ast_node* elseactions)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_IF;
    ast->data.ifexpr.condition = condition;
    ast->data.ifexpr.ifactions = ifactions;
    ast->data.ifexpr.elseactions = elseactions;
    
    return ast;
}

struct ast_node* ast_make_while(struct ast_node* condition, struct ast_node* loopactions)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_WHILE;
    ast->data.whileexpr.condition = condition;
    ast->data.whileexpr.loopactions = loopactions;
    
    return ast;
}

struct ast_node* ast_make_for(char* cursorname, struct ast_node* begin, struct ast_node* end, struct ast_node* loopactions)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_FOR;
    ast->data.forexpr.cursorname = cursorname;
    ast->data.forexpr.begin = begin;
    ast->data.forexpr.end = end;
    ast->data.forexpr.loopactions = loopactions;
    
    return ast;
}

struct ast_node* ast_make_repeat(struct ast_node* count, struct ast_node* loopactions)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_REPEAT;
    ast->data.repeatexpr.count = count;
    ast->data.repeatexpr.loopactions = loopactions;
    
    return ast;
}

struct ast_node* ast_make_function(char* name, struct ast_node* params, struct ast_node* body)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_FUNC;
    ast->data.funcexpr.name = strdup(name);
    ast->data.funcexpr.params = params;
    ast->data.funcexpr.body = body;
    
    return ast;
}

struct ast_node* ast_make_symref(char* name)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_SYMREF;
    ast->data.symrefexpr.name = strdup(name);
    
    return ast;
}

struct ast_node* ast_make_assign(char* name, struct ast_node* val)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_ASSIGN;
    ast->data.assignexpr.name = strdup(name);
    ast->data.assignexpr.val = val;
    
    return ast;
}

struct ast_node* ast_make_spfunc(spfunc_type type, struct ast_node* left, struct ast_node* right)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_SPFUNC;
    ast->data.spfuncexpr.left = left;
    ast->data.spfuncexpr.right = right;
    ast->data.spfuncexpr.type = type;
    
    return ast;
}

struct ast_node* ast_make_turtle(turt_action_type type, struct ast_node* param)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_TURTLE;
    ast->data.turtleexpr.param = param;
    ast->data.turtleexpr.type = type;
    
    return ast;
}

struct ast_node* ast_make_integer(int intval)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_INTEGER;
    ast->data.intval = intval;
    
    return ast;
}

struct ast_node* ast_make_float(float fltval)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_FLOAT;
    ast->data.fltval = fltval;
    
    return ast;
}

struct ast_node* ast_make_string(char* strval)
{
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_STRING;
    ast->data.strval = strdup(strval);
    
    return ast;
}

/*
 * AST EXECUTION API
 */

void ast_run(struct exec_env* env, struct ast_node* ast)
{
    if(env->hasReturned == true || ast == NULL)
    {
        return;
    }
    
    /*
     * We execute each node of our AST depending of its type.
     */
    
    if(ast->type == AST_STATEMENTS)
    {
        /* Do you smell the stack overflow over there? */
        ast_run(env, ast->data.expr.left);
        ast_run(env, ast->data.expr.right);
    }
    else if(ast->type == AST_EXIT)
    {
        /* TODO end current run level */
        env->shouldExit = true;
    }
    else if(ast->type == AST_SHOWHELP)
    {
        SDL_TerminalPrint(env->term, "Write me!\n");
    }
    else if(ast->type == AST_ECHO)
    {
        SDL_TerminalPrint(env->term, "%s\n", ast_eval_as_string(env, ast->data.expr.left));
    }
    else if(ast->type == AST_TURTLE)
    {
        turt_action_type tt_action = ast->data.turtleexpr.type;
        
        if(tt_action == TURT_FORWARD)
        {
            int param = ast_eval_as_int(env, ast->data.turtleexpr.param);
            if(param == 0)
            {
                param = 20;
            }
            TT_Forward(env->turt, param);
        }
        else if(tt_action == TURT_BACKWARD)
        {
            int param = ast_eval_as_int(env, ast->data.turtleexpr.param);
            if(param == 0)
            {
                param = 20;
            }
            TT_Backward(env->turt, param);
        }
        else if(tt_action == TURT_LEFT)
        {
            float param = ast_eval_as_float(env, ast->data.turtleexpr.param);
            if(param == 0.0f)
            {
                param = 90.0f;
            }
            TT_Left(env->turt, param);
        }
        else if(tt_action == TURT_RIGHT)
        {
            float param = ast_eval_as_float(env, ast->data.turtleexpr.param);
            if(param == 0.0f)
            {
                param = 90.0f;
            }
            TT_Right(env->turt, param);
        }
        else if(tt_action == TURT_PENDOWN)
        {
            TT_PenDown(env->turt);
        }
        else if(tt_action == TURT_PENUP)
        {
            TT_PenUp(env->turt);
        }
        else if(tt_action == TURT_HIDE)
        {
            TT_HideTurtle(env->turt);
        }
        else if(tt_action == TURT_SHOW)
        {
            TT_ShowTurtle(env->turt);
        }
        else if(tt_action == TURT_WRITE)
        {
            char* param = ast_eval_as_string(env, ast->data.turtleexpr.param);
            TT_WriteText(env->turt, param);
        }
        else if(tt_action == TURT_CENTERED_CIRCLE)
        {
            int param = ast_eval_as_int(env, ast->data.turtleexpr.param);
            if(param == 0)
            {
                param = 20;
            }
            TT_CenteredCircle(env->turt, param);
        }
        else if(tt_action == TURT_CIRCLE)
        {
            int param = ast_eval_as_int(env, ast->data.turtleexpr.param);
            if(param == 0)
            {
                param = 20;
            }
            TT_Circle(env->turt, param);
        }
        else if(tt_action == TURT_HOME)
        {
            TT_Home(env->turt);
        }
        else if(tt_action == TURT_CLEAR)
        {
            TT_Clear(env->turt);
        }
        else if(tt_action == TURT_RESET)
        {
            TT_Reset(env->turt);
        }
        else /* invalid turt_action */
        {
            fprintf(stderr, "*** FATAL: invalid turt_action\n");
            exit(EXIT_FAILURE);
        }
    }
    else if(ast->type == AST_ASSIGN)
    {
        /*
         * This language does currently not have variable scopes.
         * All variables are global.
         */
        float val = ast_eval_as_float(env, ast->data.assignexpr.val);
        var_set(env, ast->data.assignexpr.name, val);
    }
    else if(ast->type == AST_IF)
    {
        if(ast_eval_boolexpr(env, ast->data.ifexpr.condition))
        {
            ast_run(env, ast->data.ifexpr.ifactions);
        }
        else
        {
            ast_run(env, ast->data.ifexpr.elseactions);
        }
    }
    else if(ast->type == AST_WHILE)
    {
        while(ast_eval_boolexpr(env, ast->data.whileexpr.condition))
        {
            ast_run(env, ast->data.whileexpr.loopactions);
        }
    }
    else if(ast->type == AST_FOR)
    {
        int i = ast_eval_as_int(env, ast->data.forexpr.begin);
        int end = ast_eval_as_int(env, ast->data.forexpr.end);
        
        while(i <= end)
        {
            var_set(env,  ast->data.forexpr.cursorname, (float) i);
            ast_run(env, ast->data.forexpr.loopactions);
            i ++;
        }
        
        var_set(env, ast->data.forexpr.cursorname, (float) i);
    }
    else if(ast->type == AST_REPEAT)
    {
        int count = ast_eval_as_int(env, ast->data.repeatexpr.count);
        int i = 1;

        while(i <= count)
        {
            ast_run(env, ast->data.repeatexpr.loopactions);
            i ++;
        }
    }
    else if(ast->type == AST_LOADFILE)
    {
        struct ast_node* ast2 = scan_file(ast_eval_as_string(env, ast->data.expr.left));

        ast_run(env, ast2);
        ast_destroy(ast2);
    }
    else if(ast->type == AST_CALL)
    {
        /* Lookup Function in Symbol Table */
        char* name = ast_eval_as_string(env, ast->data.expr.left);
        struct var_list* var = var_get(env, name);
        
        if(var == NULL)
        {
            SDL_TerminalPrint(env->term, "-!- Undefined function: %s\n", name);
            return;
        }
        
        if(!(var->isFunc))
        {
            SDL_TerminalPrint(env->term, "-!- %s: is not a function\n", name);
            return;
        }
        
        /* Prepare Environment */
        struct exec_env env2;
        env2.screen = env->screen;
        env2.turt = env->turt;
        env2.term = env->term;
        /*env2.vars = copy_vars(env->vars);*/
        env2.vars = malloc(sizeof(struct var_list) * env->max_vars);
        /* TODO find a way to copy the current symbol table */
        env2.max_vars = env->max_vars;
        env2.scope = env->scope + 1;
        env2.hasReturned = false;
        env2.shouldExit = false;
        
        /* Push Params */
        struct ast_node* cursor = ast->data.expr.right;
        int i = 0;
        while(cursor != NULL && cursor->type == AST_EXPRS)
        {
            if(i >= var->func.argc) break;
            
            char* param_name = var->func.argv[i];
            float param_val = ast_eval_as_float(env, cursor->data.expr.left);
            var_set(&env2, param_name, param_val);
            
            i++;
            cursor = cursor->data.expr.right;
        }
        
        /* Call Function */
        ast_run(&env2, var->func.body);
        
        /* Cleanup */
        free(env2.vars);
        if(env2.shouldExit == true)
        {
            /* Propagate Exit */
            env->shouldExit = true;
        }
    }
    else if(ast->type == AST_RETURN)
    {
        if(ast->data.expr.left != NULL)
        {
            env->returnValue = ast_eval_as_float(env, ast->data.expr.left);
        }
        
        env->hasReturned = true;
    }
    else if(ast->type == AST_FUNC)
    {
        /* Count Args */
        int arg_count = 0;
        struct ast_node* cursor = ast->data.funcexpr.params;
        
        while(cursor != NULL && cursor->type == AST_PARAM)
        {
            arg_count ++;
            cursor = cursor->data.expr.right;
        }
        
        /*printf("arg_count = %d\n", arg_count);*/
        
        /* Make Arg Vector */
        char** arg_vector = NULL;
        if(arg_count > 0)
        {
            arg_vector = malloc(sizeof(char*) * arg_count);
            
            cursor = ast->data.funcexpr.params;
            int i = 0;
            
            while(cursor != NULL && cursor->type == AST_PARAM)
            {
                if(i >= arg_count) break;
                arg_vector[i] = ast_eval_as_string(env, cursor->data.expr.left);
                /*printf("arg_vector[%d] = %s\n", i, arg_vector[i]);*/
                
                i ++;
                cursor = cursor->data.expr.right;
            }
        }
        
        /* Put Function to Symbol Table */
        func_set(env, ast->data.funcexpr.name, arg_count, arg_vector, ast->data.funcexpr.body);
    }
    else
    {
        fprintf(stderr, "*** FATAL: invalid AST node for EXEC context\n");
        exit(EXIT_FAILURE);
    }
}

bool ast_eval_boolexpr(struct exec_env* env, struct ast_node* ast)
{
    if(ast == NULL)
    {
        fprintf(stderr, "*** FATAL: AST node for EVAL_BOOLEXPR context is NULL!\n");
        exit(EXIT_FAILURE);
    }
    if(ast->type == AST_BOOLEXPR)
    {
        float left = ast_eval_as_float(env, ast->data.boolexpr.left);
        float right = ast_eval_as_float(env, ast->data.boolexpr.right);
        switch(ast->data.boolexpr.op)
        {
        case OP_EQ:
            return left == right;
            break;
        case OP_NEQ:
            return left != right;
            break;
        case OP_LESS:
            return left < right;
            break;
        case OP_GREATER:
            return left > right;
            break;
        case OP_LEQ:
            return left <= right;
            break;
        case OP_GEQ:
            return left >= right;
            break;
        default:
            fprintf(stderr, "*** FATAL: Invalid relational operation\n");
            exit(EXIT_FAILURE);
            break;
        }
    }
    else if(ast->type == AST_NOT)
    {
        return !(ast_eval_boolexpr(env, ast->data.expr.left));
    }
    else if(ast->type == AST_AND)
    {
        return ast_eval_boolexpr(env, ast->data.expr.left) && ast_eval_boolexpr(env, ast->data.expr.right);
    }
    else if(ast->type == AST_OR)
    {
        return ast_eval_boolexpr(env, ast->data.expr.left) || ast_eval_boolexpr(env, ast->data.expr.right);
    }
    else
    {
        fprintf(stderr, "*** FATAL: Invalid boolexpr node\n");
        exit(EXIT_FAILURE);
    }
}

int ast_eval_as_int(struct exec_env* env, struct ast_node* ast)
{
    if(ast == NULL)
    {
        fprintf(stderr, "*** FATAL: AST node for EVAL_INT context is NULL!\n");
        exit(EXIT_FAILURE);
    }
    
    if(ast->type == AST_INTEGER)
    {
        return ast->data.intval;
    }
    else
    {
        /* meh */
        float fltval = ast_eval_as_float(env, ast);
        /*if(fltval == INFINITY)
        {
            return 0;
        }*/
        return (int) fltval;
    }
}

float ast_eval_as_float(struct exec_env* env, struct ast_node* ast)
{
    if(ast == NULL)
    {
        fprintf(stderr, "*** FATAL: AST node for EVAL_FLOAT context is NULL!\n");
        exit(EXIT_FAILURE);
    }
    
    if(ast->type == AST_FLOAT)
    {
        return ast->data.fltval;
    }
    else if(ast->type == AST_INTEGER)
    {
        return (float) ast->data.intval;
    }
    if(ast->type == AST_PLUS)
    {
        return ast_eval_as_float(env, ast->data.expr.left) + ast_eval_as_float(env, ast->data.expr.right);
    }
    else if(ast->type == AST_MINUS)
    {
        return ast_eval_as_float(env, ast->data.expr.left) - ast_eval_as_float(env, ast->data.expr.right);
    }
    else if(ast->type == AST_TIMES)
    {
        return ast_eval_as_float(env, ast->data.expr.left) * ast_eval_as_float(env, ast->data.expr.right);
    }
    else if(ast->type == AST_DIV)
    {
        float right = ast_eval_as_float(env, ast->data.expr.right);
        if(right == 0.0f)
        {
            SDL_TerminalPrint(env->term, "-!- Division by zero will result in undefined behaviour!\n");
            /*return INFINITY;*/
            return 0.0f;
        }
        return ast_eval_as_float(env, ast->data.expr.left) / right;
    }
    else if(ast->type == AST_MOD)
    {
        /*return ast_eval_as_float(env, ast->data.expr.left) % ast_eval_as_float(env, ast->data.expr.right);*/
        float right = ast_eval_as_float(env, ast->data.expr.right);
        if(right == 0.0f)
        {
            SDL_TerminalPrint(env->term, "-!- Modulo by zero will result in undefined behaviour!\n");
            return 0.0f;
        }
        return (float) fmod(ast_eval_as_float(env, ast->data.expr.left), right);
    }
    else if(ast->type == AST_UNARY_MINUS)
    {
        return 0.0f - ast_eval_as_float(env, ast->data.expr.left);
    }
    else if(ast->type == AST_SYMREF)
    {
        struct var_list* var = var_get(env, strdup(ast->data.symrefexpr.name));
        if(var == NULL)
        {
            SDL_TerminalPrint(env->term, "-!- Undefined variable %s, defaulting to 0!\n", ast->data.symrefexpr.name);
            return 0.0f;
        }
        return var->val;
    }
    else if(ast->type == AST_SPFUNC)
    {
        spfunc_type type = ast->data.spfuncexpr.type;
        struct ast_node* left = ast->data.spfuncexpr.left;
        struct ast_node* right = ast->data.spfuncexpr.right;
        
        if(type == SPFUNC_COS)
        {
            return (float) cos(ast_eval_as_float(env, left) * RAD2DEG);
        }
        else if(type == SPFUNC_SIN)
        {
            return (float) sin(ast_eval_as_float(env, left) * RAD2DEG);
        }
        else if(type == SPFUNC_TAN)
        {
            return (float) tan(ast_eval_as_float(env, left) * RAD2DEG);
        }
        else if(type == SPFUNC_ABS)
        {
            return (float) fabs(ast_eval_as_float(env, left));
        }
        else if(type == SPFUNC_SQRT)
        {
            return (float) sqrt(ast_eval_as_float(env, left));
        }
        else if(type == SPFUNC_LOG)
        {
            return (float) log(ast_eval_as_float(env, left));
        }
        else if(type == SPFUNC_LOG10)
        {
            return (float) log10(ast_eval_as_float(env, left));
        }
        else if(type == SPFUNC_EXP)
        {
            return (float) exp(ast_eval_as_float(env, left));
        }
        else if(type == SPFUNC_RMDR)
        {
            float rightval = ast_eval_as_float(env, right);
            if(rightval == 0.0f)
            {
                SDL_TerminalPrint(env->term, "-!- Division by zero will result in undefined behaviour!\n");
                return 0.0f;
            }
            float r = (float) fmod(ast_eval_as_float(env, ast->data.expr.left), rightval);
            return r < 0 ? r + rightval : r;
        }
        else if(type == SPFUNC_MAX)
        {
            float leftval = ast_eval_as_float(env, left);
            float rightval = ast_eval_as_float(env, right);
            return leftval > rightval ? leftval : rightval;
        }
        else if(type == SPFUNC_MIN)
        {
            float leftval = ast_eval_as_float(env, left);
            float rightval = ast_eval_as_float(env, right);
            return leftval < rightval ? leftval : rightval;
        }
        else if(type == SPFUNC_CEIL)
        {
            return (float) ceil(ast_eval_as_float(env, left));
        }
        else if(type == SPFUNC_FLOOR)
        {
            return (float) floor(ast_eval_as_float(env, left));
        }
        else
        {
            fprintf(stderr, "*** FATAL: invalid spfunc_type");
            exit(EXIT_FAILURE);
        }
    }
    else if(ast->type == AST_CALL)
    {
        /* Lookup Function in Symbol Table */
        char* name = ast_eval_as_string(env, ast->data.expr.left);
        struct var_list* var = var_get(env, name);
        
        if(var == NULL)
        {
            SDL_TerminalPrint(env->term, "-!- Undefined function: %s\n", name);
            return;
        }
        
        if(!(var->isFunc))
        {
            SDL_TerminalPrint(env->term, "-!- %s: is not a function\n", name);
            return;
        }
        
        /* Prepare Environment */
        struct exec_env env2;
        env2.screen = env->screen;
        env2.turt = env->turt;
        env2.term = env->term;
        /*env2.vars = copy_vars(env->vars);*/
        env2.vars = malloc(sizeof(struct var_list) * env->max_vars);
        /* TODO find a way to copy the current symbol table */
        env2.max_vars = env->max_vars;
        env2.scope = env->scope + 1;
        env2.hasReturned = false;
        env2.shouldExit = false;
        
        /* Push Params */
        struct ast_node* cursor = ast->data.expr.right;
        int i = 0;
        while(cursor != NULL && cursor->type == AST_EXPRS)
        {
            if(i >= var->func.argc) break;
            
            char* param_name = var->func.argv[i];
            float param_val = ast_eval_as_float(env, cursor->data.expr.left);
            var_set(&env2, param_name, param_val);
            
            i++;
            cursor = cursor->data.expr.right;
        }
        
        /* Call Function */
        ast_run(&env2, var->func.body);
        
        /* Cleanup */
        free(env2.vars);
        if(env2.shouldExit == true)
        {
            /* Propagate Exit */
            env->shouldExit = true;
        }
        
        return env2.returnValue;
    }
    else
    {
        fprintf(stderr, "*** FATAL: invalid AST node for EVAL_FLOAT context");
        exit(EXIT_FAILURE);
    }
}

char* ast_eval_as_string(struct exec_env* env, struct ast_node* ast)
{
    if(ast == NULL)
    {
        fprintf(stderr, "*** FATAL: AST node for EVAL_STR context is NULL!\n");
        exit(EXIT_FAILURE);
    }
    
    char* str = malloc_or_die(1024 * sizeof(char));
    
    if(ast->type == AST_STRING)
    {
        sprintf(str, ast->data.strval);
    }
    else if(ast->type == AST_BOOLEXPR || ast->type == AST_AND || ast->type == AST_OR || ast->type == AST_NOT)
    {
        bool boolval = ast_eval_boolexpr(env, ast);
        sprintf(str, boolval == true ? "True" : "False");
    }
    else if(ast->type == AST_INTEGER)
    {
        sprintf(str, "%d", ast->data.intval);
    }
    else
    {
        float fltval = ast_eval_as_float(env, ast);
        sprintf(str, "%f", fltval);
    }
    
    return str;
}

/*
 * AST CLEANUP API
 */

void ast_destroy(struct ast_node* ast)
{
    if(ast == NULL)
    {
        return;
    }
    
    switch(ast->type)
    {
    case AST_PLUS:
    case AST_MINUS:
    case AST_TIMES:
    case AST_DIV:
    case AST_MOD:
    case AST_UNARY_MINUS:
    case AST_AND:
    case AST_OR:
    case AST_NOT:
    case AST_EXIT:
    case AST_SHOWHELP:
    case AST_ECHO:
    case AST_LOADFILE:
    case AST_STATEMENTS:
        ast_destroy(ast->data.expr.left);
        ast_destroy(ast->data.expr.right);
        break;
    case AST_BOOLEXPR:
        ast_destroy(ast->data.boolexpr.left);
        ast_destroy(ast->data.boolexpr.right);
        break;
    case AST_STRING:
        free(ast->data.strval);
        break;
    case AST_IF:
        ast_destroy(ast->data.ifexpr.condition);
        ast_destroy(ast->data.ifexpr.ifactions);
        ast_destroy(ast->data.ifexpr.elseactions);
        break;
    case AST_WHILE:
        ast_destroy(ast->data.whileexpr.condition);
        ast_destroy(ast->data.whileexpr.loopactions);
        break;
    case AST_FOR:
        ast_destroy(ast->data.forexpr.begin);
        ast_destroy(ast->data.forexpr.end);
        ast_destroy(ast->data.forexpr.loopactions);
        free(ast->data.forexpr.cursorname);
        break;
    case AST_SYMREF:
        free(ast->data.symrefexpr.name);
        break;
    case AST_ASSIGN:
        ast_destroy(ast->data.assignexpr.val);
        free(ast->data.assignexpr.name);
        break;
    case AST_SPFUNC:
        ast_destroy(ast->data.spfuncexpr.left);
        ast_destroy(ast->data.spfuncexpr.right);
        break;
    case AST_TURTLE:
        ast_destroy(ast->data.turtleexpr.param);
        break;
    default:
        break;
    }
    
    free(ast);
}
