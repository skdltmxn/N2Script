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
        if (n->handler && !n->handler(n, NULL))
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

/* Evaluate binary operations  */
int eval_binop(const node *self, node *result)
{
    node left;
    node *right = result; /* result works as both rhs and final result */

    /* Called from main flow, do nothing */
    if (!result)
        return 1;

    /* Evaluate lhs */
    if (!NODE_LEFT(self)->handler(NODE_LEFT(self), &left))
        return 0;

    /* Evaluate rhs */
    if (!NODE_RIGHT(self)->handler(NODE_RIGHT(self), right))
        return 0;

    /* Check type match */
    if (type_check(self->type, &left, right) == TYPE_CHECK_ERROR)
        return 0;

    switch (self->type)
    {
    case NODE_ADD:
        if (add_expression(&left, right, result))
            return 1;

        break;

    case NODE_SUB:
        if (sub_expression(&left, right, result))
            return 1;

        break;

    case NODE_MUL:
        if (mul_expression(&left, right, result))
            return 1;

        break;

    case NODE_DIV:
        if (div_expression(&left, right, result))
            return 1;

        break;

    default:
        break;
    }

    return 0;
}

/* Evaluate reading values */
int eval_refer(const node *self, node *result)
{
    /* Called from main flow, do nothing */
    if (!result)
        return 1;

    switch (self->type)
    {
        case NODE_INT:
            result->val.ival = self->val.ival;
            result->type = self->type;
            return 1;

        case NODE_DOUBLE:
            result->val.dval = self->val.dval;
            result->type = self->type;
            return 1;

        case NODE_STRING:
            result->val.sval = self->val.sval;
            result->type = self->type;
            return 1;

        case NODE_IDENT:
            memcpy(result, resolve_var(self), sizeof(*self));
            return 1;

        default:
            break;
    }

    return 0;
}

int eval_assign(const node *self, node *result)
{
    node rhs;

    if (!NODE_RHS(self)->handler(NODE_RHS(self), &rhs))
        return 0;

    if (!assign_var(NODE_LHS(self)->val.sval, &rhs))
        eval_error("Fatal: failed to assign `%s`\n", NODE_LHS(self)->val.sval);

    if (rhs.type == NODE_INT)
        printf("%s <- %d\n", NODE_LHS(self)->val.sval, rhs.val.ival);
    else if (rhs.type == NODE_DOUBLE)
        printf("%s <- %f\n", NODE_LHS(self)->val.sval, rhs.val.dval);
    else if (rhs.type == NODE_STRING)
        printf("%s <- %s\n", NODE_LHS(self)->val.sval, rhs.val.sval);

    if (result)
        memcpy(result, &rhs, sizeof(rhs));

    return 1;
}

int eval_if(const node *n, node *result)
{
    node cond;

    /* if (!eval_expression(n->child[0], &cond)) */
        return 0;

    if (to_boolean(&cond))
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