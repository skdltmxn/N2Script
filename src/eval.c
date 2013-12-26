/*
 *  eval.c
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file contains code for evaluating script
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "ast.h"
#include "eval.h"
#include "util.h"
#include "resource.h"

#define TYPE_NUM(expr) \
    ((expr)->type == EXP_INTEGER || (expr)->type == EXP_REAL)

int evaluate(struct statement *stmts)
{
    struct statement *stmt = stmts;

    while (stmt)
    {
        if (!stmt->execute(stmt))
            return 0;

        stmt = stmt->next;
    }

    return 1;
}

/*
 * 1) number + number
 * 2) string + string
 */
int add_expression(const struct expression *left,
                   const struct expression *right,
                   struct expression *result)
{
    if (!left || !right || !result)
        return 0;

    /* STR + STR */
    if (left->type == EXP_STRING && right->type == EXP_STRING)
    {
        byte *new_string = 
            string_concat(left->value.string, right->value.string);

        result->value.string = register_rsrc_string(new_string);

        if (new_string != result->value.string)
            safe_free(new_string);

        result->type = EXP_STRING;

        return 1;
    }
    /* NUM + NUM */
    else if (TYPE_NUM(left) && TYPE_NUM(right))
    {
        /* FLOAT + ? = FLOAT */
        if (left->type == EXP_REAL || right->type == EXP_REAL)
        {
            result->value.real =
                ((left->type == EXP_REAL) ? left->value.real : left->value.integer) +
                ((right->type == EXP_REAL) ? right->value.real : right->value.integer);
            result->type = EXP_REAL;

            return 1;
        }
        /* INT + INT = INT */
        else
        {
            result->value.integer = left->value.integer + right->value.integer;
            result->type = EXP_INTEGER;

            return 1;
        }
    }

    return 0;
}

int sub_expression(const struct expression *left,
                   const struct expression *right,
                   struct expression *result)
{
    if (TYPE_NUM(left) && TYPE_NUM(right))
    {
        /* FLOAT + ? = FLOAT */
        if (left->type == EXP_REAL || right->type == EXP_REAL)
        {
            result->value.real =
                ((left->type == EXP_REAL) ? left->value.real : left->value.integer) -
                ((right->type == EXP_REAL) ? right->value.real : right->value.integer);
            result->type = EXP_REAL;

            return 1;
        }
        /* INT + INT = INT */
        else
        {
            result->value.integer = left->value.integer - right->value.integer;
            result->type = EXP_INTEGER;

            return 1;
        }
    }

    return 0;
}

int mul_expression(const struct expression *left,
                   const struct expression *right,
                   struct expression *result)
{
    if (!left || !right || !result)
        return 0;

    /* INTEGER * STR */
    if ((left->type == EXP_INTEGER && right->type == EXP_STRING) ||
        (left->type == EXP_STRING && right->type == EXP_INTEGER))
    {
        int count = (left->type == EXP_INTEGER) ?
            left->value.integer : right->value.integer;
        const byte *str = (left->type == EXP_STRING) ?
            left->value.string : right->value.string;
        byte *new_string = string_repeat(str, count);

        result->value.string = register_rsrc_string(new_string);
        if (new_string != result->value.string)
            safe_free(new_string);

        result->type = EXP_STRING;

        return 1;
    }
    /* NUM * NUM */
    else if (TYPE_NUM(left) && TYPE_NUM(right))
    {
        /* FLOAT * ? = FLOAT */
        if (left->type == EXP_REAL || right->type == EXP_REAL)
        {
            result->value.real =
                ((left->type == EXP_REAL) ? left->value.real : left->value.integer) *
                ((right->type == EXP_REAL) ? right->value.real : right->value.integer);
            result->type = EXP_REAL;

            return 1;
        }
        /* INT * INT = INT */
        else
        {
            result->value.integer =
                left->value.integer * right->value.integer;
            result->type = EXP_INTEGER;

            return 1;
        }
    }

    return 0;
}

int div_expression(const struct expression *left,
                   const struct expression *right,
                   struct expression *result)
{
    if (!left || !right || !result)
        return 0;

    if (TYPE_NUM(left) && TYPE_NUM(right))
    {
        /* FLOAT / ? = FLOAT */
        if (left->type == EXP_REAL || right->type == EXP_REAL)
        {
            result->value.real =
                ((left->type == EXP_REAL) ? left->value.real : left->value.integer) /
                ((right->type == EXP_REAL) ? right->value.real : right->value.integer);
            result->type = EXP_REAL;

            return 1;
        }
        /* INT / INT = INT */
        else
        {
            result->value.integer = left->value.integer / right->value.integer;
            result->type = EXP_INTEGER;

            return 1;
        }
    }

    return 0;
}

/*
 * return
 *   1: ok
 *   0: pending
 *   -1: error
 */
int type_check(const enum expr_type type,
               const struct expression *left,
               const struct expression *right)
{
    if (!left || !right)
        return TYPE_CHECK_ERROR;

    if (left->type > EXP_STRING || right->type > EXP_STRING)
        return TYPE_CHECK_PENDING;

    /* check for primitive types */
    switch (type)
    {
    /*
     * string + string
     * number + number
     */
    case EXP_ADD:
        if (left->type == EXP_STRING && right->type == EXP_STRING)
            break;
        if (left->type == EXP_STRING && right->type != EXP_STRING)
            return TYPE_CHECK_ERROR;
        if (!TYPE_NUM(left) || !TYPE_NUM(right))
            return TYPE_CHECK_ERROR;
        break;

    /* number and number */
    case EXP_DIV:
    case EXP_SUB:
        if (!TYPE_NUM(left) || !TYPE_NUM(right))
            return TYPE_CHECK_ERROR;
        break;

    /*
     * string * integer
     * number * number
     */
    case EXP_MUL:
        if (left->type == EXP_STRING && right->type != EXP_INTEGER)
            return TYPE_CHECK_ERROR;
        if (left->type == EXP_REAL && right->type == EXP_STRING)
            return TYPE_CHECK_ERROR;
        break;

    default:
        return TYPE_CHECK_ERROR;
    }

    return TYPE_CHECK_OK;
}

/*
 * Convert expression to boolean
 */
static int to_boolean(const struct expression *expr)
{
    if (!expr)
        return 0;

    switch (expr->type)
    {
    case EXP_INTEGER:
        return (expr->value.integer != 0);

    case EXP_REAL:
        return (expr->value.real != 0.0);

    case EXP_STRING:
        return (expr->value.string[0] != '\0');

    /* all other types cannot be converted */
    default:
        return 0;
    }
}

static struct expression *eval_expression(struct expression *expr,
                                          struct expression *result)
{
    struct expression *left = NULL;
    struct expression *right = NULL;
    struct expression save;

    if (!expr || !result)
        return NULL;

    switch (expr->type)
    {
    case EXP_ADD:
        left = eval_expression(expr->left, result);
        memcpy(&save, left, sizeof(save));
        right = eval_expression(expr->right, result);

        if (!left || !right)
            break;

        if (type_check(expr->type, left, right) == TYPE_CHECK_ERROR)
            break;

        if (add_expression(&save, right, result))
            return result;

        break;

    case EXP_SUB:
        left = eval_expression(expr->left, result);
        memcpy(&save, left, sizeof(save));
        right = eval_expression(expr->right, result);

        if (!left || !right)
            break;

        if (type_check(expr->type, left, right) == TYPE_CHECK_ERROR)
            break;

        if (sub_expression(&save, right, result))
            return result;

        break;

    case EXP_MUL:
        left = eval_expression(expr->left, result);
        memcpy(&save, left, sizeof(save));
        right = eval_expression(expr->right, result);

        if (!left || !right)
            break;

        if (type_check(expr->type, left, right) == TYPE_CHECK_ERROR)
            break;

        if (mul_expression(&save, right, result))
            return result;

        break;

    case EXP_DIV:
        left = eval_expression(expr->left, result);
        memcpy(&save, left, sizeof(save));
        right = eval_expression(expr->right, result);

        if (!left || !right)
            break;

        if (type_check(expr->type, left, right) == TYPE_CHECK_ERROR)
            break;

        if (div_expression(&save, right, result))
            return result;

        break;

    case EXP_INTEGER:
        result->value.integer = expr->value.integer;
        result->type = expr->type;
        return expr;

    case EXP_REAL:
        result->value.real = expr->value.real;
        result->type = expr->type;
        return expr;

    case EXP_STRING:
        result->value.string = expr->value.string;
        result->type = expr->type;
        return expr;

    case EXP_IDENT:
        memcpy(result, resolve_var(expr), sizeof(struct expression));
        return result;
    }

    return NULL;
}

int eval_assign(const struct statement *stmt)
{
    struct assign_stmt *assign = stmt->assign;
    struct expression result;

    if (!assign)
        return 0;

    if (!eval_expression(assign->expr, &result))
        return 0;

    result.vtbl = assign->expr->vtbl;
    if (!assign_var(assign->ident, &result))
        eval_error("Fatal: failed to assign `%s`\n", assign->ident);

    if (result.type == EXP_INTEGER)
        printf("%s <- %d\n", assign->ident, result.value.integer);
    else if (result.type == EXP_REAL)
        printf("%s <- %f\n", assign->ident, result.value.real);
    else if (result.type == EXP_STRING)
        printf("%s <- %s\n", assign->ident, result.value.string);

    return 1;
}

int eval_if(const struct statement *stmt)
{
    struct if_stmt *ifstmt = stmt->ifstmt;
    struct expression result;

    if (!ifstmt)
        return 0;

    if (!eval_expression(ifstmt->condition, &result))
        return 0;

    if (to_boolean(&result))
        evaluate(ifstmt->true_stmts);
    else
        evaluate(ifstmt->false_stmts);
    
    return 1;
}

void eval_error(const char *s, ...)
{
    va_list ap;

    va_start(ap, s);
    vfprintf(stderr, s, ap);
    va_end(ap);

    destroy_all();
    exit(-3);
}