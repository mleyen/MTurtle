#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "MTurtle.h"
#include "consolev2_common.h"

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
        /* Already Defined Variable -- Do Nothing */
    }

    cursor->val = val;
    return cursor;
}

/*
 * AST GENERATION API
 */

struct ast_node* ast_make(ast_type type, struct ast_node* left, struct ast_node* right)
{
    //printf("ast_make() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = type;
    ast->data.expr.left = left;
    ast->data.expr.right = right;
    
    /* should be enough */
    return ast;
}

struct ast_node* ast_make_boolexpr(boolop_type op, struct ast_node* left, struct ast_node* right)
{
    //printf("ast_make_boolexpr() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_BOOLEXPR;
    ast->data.boolexpr.left = left;
    ast->data.boolexpr.right = right;
    ast->data.boolexpr.op = op;
    
    return ast;
}

struct ast_node* ast_make_if(struct ast_node* condition, struct ast_node* ifactions, struct ast_node* elseactions)
{
    //printf("ast_make_if() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_IF;
    ast->data.ifexpr.condition = condition;
    ast->data.ifexpr.ifactions = ifactions;
    ast->data.ifexpr.elseactions = elseactions;
    
    return ast;
}

struct ast_node* ast_make_while(struct ast_node* condition, struct ast_node* loopactions)
{
    //printf("ast_make_while() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_WHILE;
    ast->data.whileexpr.condition = condition;
    ast->data.whileexpr.loopactions = loopactions;
    
    return ast;
}

struct ast_node* ast_make_for(char* cursorname, struct ast_node* begin, struct ast_node* end, struct ast_node* loopactions)
{
    //printf("ast_make_for() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_FOR;
    ast->data.forexpr.cursorname = cursorname;
    ast->data.forexpr.begin = begin;
    ast->data.forexpr.end = end;
    ast->data.forexpr.loopactions = loopactions;
    
    return ast;
}

struct ast_node* ast_make_symref(char* name)
{
    //printf("ast_make_symref() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_SYMREF;
    ast->data.symrefexpr.name = strdup(name);
    
    return ast;
}

struct ast_node* ast_make_assign(char* name, struct ast_node* val)
{
    //printf("ast_make_assign() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));

    ast->type = AST_ASSIGN;
    ast->data.assignexpr.name = strdup(name);
    ast->data.assignexpr.val = val;

    return ast;
}

struct ast_node* ast_make_spfunc(spfunc_type type, struct ast_node* left, struct ast_node* right)
{
    //printf("ast_make_spfunc() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_SPFUNC;
    ast->data.spfuncexpr.left = left;
    ast->data.spfuncexpr.right = right;
    ast->data.spfuncexpr.type = type;
    
    return ast;
}

struct ast_node* ast_make_turtle(turt_action_type type, struct ast_node* param)
{
    //printf("ast_make_turtle() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_TURTLE;
    ast->data.turtleexpr.param = param;
    ast->data.turtleexpr.type = type;
    
    return ast;
}

struct ast_node* ast_make_integer(int intval)
{
    //printf("ast_make_integer() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_INTEGER;
    ast->data.intval = intval;
    
    return ast;
}

struct ast_node* ast_make_float(float fltval)
{
    //printf("ast_make_float() is called\n");
    struct ast_node* ast = malloc_or_die(sizeof(struct ast_node));
    
    ast->type = AST_FLOAT;
    ast->data.fltval = fltval;
    
    return ast;
}

struct ast_node* ast_make_string(char* strval)
{
    //printf("ast_make_string() is called\n");
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
    if(ast == NULL)
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
    else if(ast->type == AST_LOADFILE)
    {
        SDL_TerminalPrint(env->term, "Not yet implemented...\n");
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

int ast_eval_as_float(struct exec_env* env, struct ast_node* ast)
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
            return (float) cos(ast_eval_as_float(env, left));
        }
        else if(type == SPFUNC_SIN)
        {
            return (float) sin(ast_eval_as_float(env, left));
        }
        else if(type == SPFUNC_TAN)
        {
            return (float) tan(ast_eval_as_float(env, left));
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
 * PARSER API
 */

int readLine(FILE* file, char* buf, size_t max_size, struct exec_env* env)
{
    if(file == NULL /*|| file == stdin*/) /* main interpreter loop (i.e. not in loaded file) */
    {
        printf("yyin is null\n");
        /*fprintf(stderr, "We should not be there right now!\n");
        exit(0);*/
        /* Buffer is Set to Typed Command (from SDL terminal event) */
        size_t len = strlen(env->command);
        memcpy(buf, env->command, len + 1);

        fprintf(stderr, "buf=%s env->command=%s len=%d\n", buf, env->command, len);

        return len;
    }

    else /* non interactive mode */
    {
        printf("yyin is not null\n");
        if(file == stdin)
        {
            printf("Reading from stdin\n");
        }

        /* Read File -> Fill Buffer */
        size_t len = fread(buf, 1, max_size, file);
        printf("Read %d chars\n", len);

        return len;
    }
}
