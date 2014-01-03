/*
 *  eval.c
 *
 *    Copyright (c) 2013-2014 skdltmxn <supershop@naver.com>
 *
 *  This file contains code for evaluating script
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "types.h"
#include "node.h"
#include "variable.h"
#include "eval.h"
#include "util.h"
#include "resource.h"

#define TYPE_NUM(node) \
    ((node)->type == NODE_INT || (node)->type == NODE_DOUBLE)

int evaluate(node *nodes)
{
    node *n = nodes;

    while (n)
    {
        if (n->handler && !n->handler(n))
            return 0;

        n = n->next;
    }

    return 1;
}

/*
 * 1) number + number
 * 2) string + string
 */
int add_expression(const node *left, const node *right, node *result)
{
    /* STR + STR */
    if (left->type == NODE_STRING && right->type == NODE_STRING)
    {
        byte *new_string = 
            string_concat(left->val.sval, right->val.sval);

        result->val.sval = register_rsrc_string(new_string);

        if (new_string != result->val.sval)
            safe_free(new_string);

        result->type = NODE_STRING;

        return 1;
    }
    /* NUM + NUM */
    else if (TYPE_NUM(left) && TYPE_NUM(right))
    {
        /* FLOAT + ? = FLOAT */
        if (left->type == NODE_DOUBLE || right->type == NODE_DOUBLE)
        {
            result->val.dval =
                ((left->type == NODE_DOUBLE) ? left->val.dval : left->val.ival) +
                ((right->type == NODE_DOUBLE) ? right->val.dval : right->val.ival);
            result->type = NODE_DOUBLE;

            return 1;
        }
        /* INT + INT = INT */
        else
        {
            result->val.ival = left->val.ival + right->val.ival;
            result->type = NODE_INT;

            return 1;
        }
    }

    return 0;
}

int sub_expression(const node *left, const node *right, node *result)
{
    if (TYPE_NUM(left) && TYPE_NUM(right))
    {
        /* FLOAT + ? = FLOAT */
        if (left->type == NODE_DOUBLE || right->type == NODE_DOUBLE)
        {
            result->val.dval =
                ((left->type == NODE_DOUBLE) ? left->val.dval : left->val.ival) -
                ((right->type == NODE_DOUBLE) ? right->val.dval : right->val.ival);
            result->type = NODE_DOUBLE;

            return 1;
        }
        /* INT + INT = INT */
        else
        {
            result->val.ival = left->val.ival - right->val.ival;
            result->type = NODE_INT;

            return 1;
        }
    }

    return 0;
}

int mul_expression(const node *left, const node *right, node *result)
{
    /* INTEGER * STR */
    if ((left->type == NODE_INT && right->type == NODE_STRING) ||
        (left->type == NODE_STRING && right->type == NODE_INT))
    {
        int count = (left->type == NODE_INT) ?
            left->val.ival : right->val.ival;
        const byte *str = (left->type == NODE_STRING) ?
            left->val.sval : right->val.sval;
        byte *new_string = string_repeat(str, count);

        result->val.sval = register_rsrc_string(new_string);
        if (new_string != result->val.sval)
            safe_free(new_string);

        result->type = NODE_STRING;

        return 1;
    }
    /* NUM * NUM */
    else if (TYPE_NUM(left) && TYPE_NUM(right))
    {
        /* FLOAT * ? = FLOAT */
        if (left->type == NODE_DOUBLE || right->type == NODE_DOUBLE)
        {
            result->val.dval =
                ((left->type == NODE_DOUBLE) ? left->val.dval : left->val.ival) *
                ((right->type == NODE_DOUBLE) ? right->val.dval : right->val.ival);
            result->type = NODE_DOUBLE;

            return 1;
        }
        /* INT * INT = INT */
        else
        {
            result->val.ival = left->val.ival * right->val.ival;
            result->type = NODE_INT;

            return 1;
        }
    }

    return 0;
}

int div_expression(const node *left, const node *right, node *result)
{
    if (TYPE_NUM(left) && TYPE_NUM(right))
    {
        /* FLOAT / ? = FLOAT */
        if (left->type == NODE_DOUBLE || right->type == NODE_DOUBLE)
        {
            result->val.dval =
                ((left->type == NODE_DOUBLE) ? left->val.dval : left->val.ival) /
                ((right->type == NODE_DOUBLE) ? right->val.dval : right->val.ival);
            result->type = NODE_DOUBLE;

            return 1;
        }
        /* INT / INT = INT */
        else
        {
            result->val.ival = left->val.ival / right->val.ival;
            result->type = NODE_INT;

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
int type_check(const node_type type, const node *left, const node *right)
{
    if (left->type > NODE_STRING || right->type > NODE_STRING)
        return TYPE_CHECK_PENDING;

    /* check for primitive types */
    switch (type)
    {
    /*
     * string + string
     * number + number
     */
    case NODE_ADD:
        if (left->type == NODE_STRING && right->type == NODE_STRING)
            break;
        if (left->type == NODE_STRING && right->type != NODE_STRING)
            return TYPE_CHECK_ERROR;
        if (!TYPE_NUM(left) || !TYPE_NUM(right))
            return TYPE_CHECK_ERROR;
        break;

    /* number and number */
    case NODE_DIV:
    case NODE_SUB:
        if (!TYPE_NUM(left) || !TYPE_NUM(right))
            return TYPE_CHECK_ERROR;
        break;

    /*
     * string * integer
     * number * number
     */
    case NODE_MUL:
        if (left->type == NODE_STRING && right->type != NODE_INT)
            return TYPE_CHECK_ERROR;
        if (left->type == NODE_DOUBLE && right->type == NODE_STRING)
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
static int to_boolean(const node *n)
{
    switch (n->type)
    {
    case NODE_INT:
        return (n->val.ival != 0);

    case NODE_DOUBLE:
        return (n->val.dval != 0.0);

    case NODE_STRING:
        return (n->val.sval[0] != '\0');

    /* all other types cannot be converted */
    default:
        return 0;
    }
}

static node *eval_expression(node *n, node *result)
{
    node *left = NULL;
    node *right = NULL;
    node save;

    switch (n->type)
    {
    case NODE_ADD:
        left = eval_expression(NODE_LEFT(n), result);
        memcpy(&save, left, sizeof(save));
        right = eval_expression(NODE_RIGHT(n), result);

        if (!left || !right)
            break;

        if (type_check(n->type, left, right) == TYPE_CHECK_ERROR)
            break;

        if (add_expression(&save, right, result))
            return result;

        break;

    case NODE_SUB:
        left = eval_expression(NODE_LEFT(n), result);
        memcpy(&save, left, sizeof(save));
        right = eval_expression(NODE_RIGHT(n), result);

        if (!left || !right)
            break;

        if (type_check(n->type, left, right) == TYPE_CHECK_ERROR)
            break;

        if (sub_expression(&save, right, result))
            return result;

        break;

    case NODE_MUL:
        left = eval_expression(NODE_LEFT(n), result);
        memcpy(&save, left, sizeof(save));
        right = eval_expression(NODE_RIGHT(n), result);

        if (!left || !right)
            break;

        if (type_check(n->type, left, right) == TYPE_CHECK_ERROR)
            break;

        if (mul_expression(&save, right, result))
            return result;

        break;

    case NODE_DIV:
        left = eval_expression(NODE_LEFT(n), result);
        memcpy(&save, left, sizeof(save));
        right = eval_expression(NODE_RIGHT(n), result);

        if (!left || !right)
            break;

        if (type_check(n->type, left, right) == TYPE_CHECK_ERROR)
            break;

        if (div_expression(&save, right, result))
            return result;

        break;

    case NODE_INT:
        result->val.ival = n->val.ival;
        result->type = n->type;
        return n;

    case NODE_DOUBLE:
        result->val.dval = n->val.dval;
        result->type = n->type;
        return n;

    case NODE_STRING:
        result->val.sval = n->val.sval;
        result->type = n->type;
        return n;

    case NODE_IDENT:
        memcpy(result, resolve_var(n), sizeof(node));
        return result;
    }

    return NULL;
}

int eval_assign(const node *n)
{
    node result;

    if (!eval_expression(NODE_RHS(n), &result))
        return 0;

    if (!assign_var(NODE_LHS(n)->val.sval, &result))
        eval_error("Fatal: failed to assign `%s`\n", NODE_LHS(n)->val.sval);

    if (result.type == NODE_INT)
        printf("%s <- %d\n", NODE_LHS(n)->val.sval, result.val.ival);
    else if (result.type == NODE_DOUBLE)
        printf("%s <- %f\n", NODE_LHS(n)->val.sval, result.val.dval);
    else if (result.type == NODE_STRING)
        printf("%s <- %s\n", NODE_LHS(n)->val.sval, result.val.sval);

    return 1;
}

int eval_if(const node *n)
{
    node result;

    if (!eval_expression(n->child[0], &result))
        return 0;

    if (to_boolean(&result))
        evaluate(n->child[1]);
    else
        evaluate(n->child[2]);
    
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