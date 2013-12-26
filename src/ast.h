/*
 *  ast.h
 *
 *    Copyright (c) 2013 skdltmxn <supershop@naver.com>
 *
 */

#ifndef __AST_T_
#define __AST_T_

#include "statement.h"
#include "grammar.h" /* YYLTYPE */
#include "variable.h"

struct ast_tree
{
    struct statement *stmts;
    struct var_table *var_tbl;
    yyscan_t scanner;
};

extern struct ast_tree *get_ast_root();
extern struct ast_tree *init_ast();
extern void destroy_ast(struct ast_tree *root);
extern struct statement *new_statement(int (*execute)(const struct statement *stmt),
                                       void (*destroy)(struct statement *stmt));
extern struct assign_stmt *new_assign_stmt(const char *token,
                                           struct expression *expr);
extern struct if_stmt *new_if_stmt(struct expression *condition,
                                   struct statement *true_stmts,
                                   struct statement *false_stmts);
extern void destroy_assign_stmt(struct statement *stmt);
extern void destroy_if_stmt(struct statement *stmt);
extern void add_statement(struct statement *before, struct statement *after);
extern struct expression *new_expression(const enum expr_type type,
                                         const union exp_value *value,
                                         struct var_table *vtbl);
extern struct expression *new_operation(const enum expr_type type,
                                        struct expression *left,
                                        struct expression *right);
extern void parse_error(struct ast_tree *root, const char *msg);

#endif
