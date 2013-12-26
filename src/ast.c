/*
 *  ast.c
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 *  This file contains code for construction/destruction of the AST tree
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "types.h"
#include "ast.h"
#include "eval.h"
#include "util.h"

struct ast_tree *init_ast()
{
    struct ast_tree *root = NULL;

    root = (struct ast_tree *)safe_malloc(sizeof(*root));

    root->stmts = NULL;
    root->var_tbl = new_var_table(NULL);
    root->scanner = NULL;

    return root;
}

void destroy_expression(struct expression *expr)
{
    if (!expr)
        return;

    destroy_expression(expr->left);
    destroy_expression(expr->right);

    safe_free(expr);
}

void destroy_statement(struct statement *stmt)
{
    if (!stmt)
        return;

    destroy_statement(stmt->next);
    stmt->destroy(stmt);

    safe_free(stmt);
}

void destroy_ast(struct ast_tree *root)
{
    destroy_statement(root->stmts);
    destroy_var_table(root->var_tbl);

    safe_free(root);
}

struct statement *new_statement(int (*execute)(const struct statement *stmt),
                                void (*destroy)(struct statement *stmt))
{
    struct statement *stmt = NULL;

    stmt = (struct statement *)safe_malloc(sizeof(*stmt));

    stmt->next = NULL;
    stmt->execute = execute;
    stmt->destroy = destroy;

    return stmt;
}

struct assign_stmt *new_assign_stmt(const char *token, struct expression *expr)
{
    struct assign_stmt *stmt = NULL;

    stmt = (struct assign_stmt *)safe_malloc(sizeof(*stmt));

    stmt->ident = token;
    stmt->expr = expr;

    return stmt;
}

struct if_stmt *new_if_stmt(struct expression *condition,
                            struct statement *true_stmts,
                            struct statement *false_stmts)
{
    struct if_stmt *stmt = NULL;

    stmt = (struct if_stmt *)safe_malloc(sizeof(*stmt));

    stmt->condition = condition;
    stmt->true_stmts = true_stmts;
    stmt->false_stmts = false_stmts;

    return stmt;
}

void destroy_assign_stmt(struct statement *stmt)
{
    destroy_expression(stmt->assign->expr);
    safe_free(stmt->assign);
}

void destroy_if_stmt(struct statement *stmt)
{
    destroy_expression(stmt->ifstmt->condition);
    destroy_statement(stmt->ifstmt->true_stmts);
    destroy_statement(stmt->ifstmt->false_stmts);
    safe_free(stmt->ifstmt);
}

void add_statement(struct statement *before, struct statement *after)
{
    struct statement **iter = NULL;

    iter = &before;

    while (*iter) iter = &(*iter)->next;

    *iter = after;
}

struct expression *new_expression(const enum expr_type type,
                                  const union exp_value *value,
                                  struct var_table *vtbl)
{
    struct expression *expr = NULL;
    expr = (struct expression *)safe_malloc(sizeof(*expr));

    expr->left = NULL;
    expr->right = NULL;
    expr->type = type;
    memcpy(&expr->value, value, sizeof(*value));
    expr->vtbl = vtbl;

    return expr;
}

struct expression *new_operation(const enum expr_type type,
                                 struct expression *left,
                                 struct expression *right)
{
    struct expression *expr = NULL;
    int check = TYPE_CHECK_OK;

    expr = (struct expression *)safe_malloc(sizeof(*expr));

    check = type_check(type, left, right);

    /* cannot evaluate for now */
    if (check == TYPE_CHECK_PENDING)
    {
        expr->left = left;
        expr->right = right;
        expr->type = type;
        expr->vtbl = left->vtbl;
        return expr;
    }
    /* syntax error */
    else if (check == TYPE_CHECK_ERROR)
        parse_error(get_ast_root(), "type mismatch");

    /* evaluate as much as possible */
    expr->left = NULL;
    expr->right = NULL;
    expr->vtbl = left->vtbl;

    if (type == EXP_ADD)
    {
        if (add_expression(left, right, expr))
        {
            destroy_expression(left);
            destroy_expression(right);
            return expr;
        }
    }
    else if (type == EXP_SUB)
    {
        if (sub_expression(left, right, expr))
        {
            destroy_expression(left);
            destroy_expression(right);
            return expr;
        }
    }
    else if (type == EXP_MUL)
    {
        if (mul_expression(left, right, expr))
        {
            destroy_expression(left);
            destroy_expression(right);
            return expr;
        }
    }
    else if (type == EXP_DIV)
    {
        if (div_expression(left, right, expr))
        {
            destroy_expression(left);
            destroy_expression(right);
            return expr;
        }
    }

    return expr;
}

extern YYLTYPE *yyget_lloc(yyscan_t);
extern int yylex_destroy(yyscan_t scanner);
void parse_error(struct ast_tree *root, const char *msg)
{
    YYLTYPE *loc = yyget_lloc(root->scanner);
    fprintf(stderr, "Parse error: %s (line: %d, col: %d)", msg,
        loc->first_line,
        loc->last_column);
    yylex_destroy(root->scanner);
    destroy_all();
    exit(-3);
}
