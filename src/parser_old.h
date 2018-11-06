
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <logger.h>
#include <lexer.h>
#include "tokens.h"

typedef struct lab_parser_token_container_t {

    lab_lexer_token_container_t* tokens;
    size_t                       iter;

} lab_parser_token_container_t;

typedef enum lab_parser_ast_expr_kind_e {

    lab_expr_num,
    lab_expr_var,
    lab_expr_bin,
    lab_expr_call,

} lab_parser_ast_expr_kind_e;


/*
    Expressions
*/

typedef struct lab_parser_ast_expr_t {

    void* expr;
    lab_parser_ast_expr_kind_e which;
    struct lab_parser_ast_expr_t* next;

} lab_parser_ast_expr_t;

typedef struct lab_parser_ast_expr_num_t {
    int is_float;
    union {
        int64_t int_val;
        double  float_val;
    };

} lab_parser_ast_expr_num_t;

typedef struct lab_parser_ast_expr_var_t {

    char* name;

} lab_parser_ast_expr_var_t;

typedef struct lab_parser_ast_expr_bin_t {

    char op;
    lab_parser_ast_expr_t* lhs, *rhs;

} lab_parser_ast_expr_bin_t;

typedef struct lab_parser_ast_expr_call_t {

    char* callee;
    size_t argc;
    lab_parser_ast_expr_t* args;

} lab_parser_ast_expr_call_t;

/*
    Prototypes
*/

typedef struct lab_parser_ast_prototype_parameter_t {

    char* name;
    struct lab_parser_ast_prototype_parameter_t* next;

} lab_parser_ast_prototype_parameter_t;

typedef struct lab_parser_ast_prototype_t {

    char* name;
    size_t argc;
    lab_parser_ast_prototype_parameter_t* args;

    struct lab_parser_ast_prototype_t* next;


} lab_parser_ast_prototype_t;

/*
    AST
*/

typedef struct lab_parser_ast_function_t {

    lab_parser_ast_prototype_t* head;
    lab_parser_ast_expr_t* body;

    struct lab_parser_ast_function_t* next;

} lab_parser_ast_function_t;

typedef struct lab_parser_ast_t {

    lab_parser_ast_expr_t* main_exprs;
    lab_parser_ast_expr_t* last_main_expr;
    lab_parser_ast_prototype_t* prototypes;
    lab_parser_ast_function_t* functions;

} lab_parser_ast_t;

/*
    Helper functions
*/

lab_parser_ast_expr_t* lab_parser_ast_expr_make(void* expr, lab_parser_ast_expr_kind_e which) {
    lab_parser_ast_expr_t* e = (lab_parser_ast_expr_t*)malloc(sizeof(lab_parser_ast_expr_t));
    e->expr = expr;
    e->which = which;
    return e;
}

lab_parser_ast_expr_num_t* lab_parser_ast_expr_num_make(int64_t ival, double fval, int is_float) {
    lab_parser_ast_expr_num_t* n = (lab_parser_ast_expr_num_t*)malloc(sizeof(lab_parser_ast_expr_num_t));
    if(is_float) {
        n->float_val = fval;
        n->int_val = 0;
        n->is_float = 1;
    } else {
        n->float_val = 0.0;
        n->int_val = ival;
        n->is_float = 0;
    }
    return n;
}

lab_parser_ast_expr_var_t* lab_parser_ast_expr_var_make(char* name) {
    lab_parser_ast_expr_var_t* v = (lab_parser_ast_expr_var_t*)malloc(sizeof(lab_parser_ast_expr_var_t));
    v->name = name;
    return v;
}

lab_parser_ast_expr_bin_t* lab_parser_ast_expr_bin_make(char op, lab_parser_ast_expr_t* lhs, lab_parser_ast_expr_t* rhs) {
    lab_parser_ast_expr_bin_t* b = (lab_parser_ast_expr_bin_t*)malloc(sizeof(lab_parser_ast_expr_bin_t));
    b->op = op;
    b->lhs = lhs;
    b->rhs = rhs;
    return b;
}

lab_parser_ast_expr_call_t* lab_parser_ast_expr_call_make(char* callee, size_t argc, lab_parser_ast_expr_t* args) {
    lab_parser_ast_expr_call_t* c = (lab_parser_ast_expr_call_t*)malloc(sizeof(lab_parser_ast_expr_call_t));
    c->callee = callee;
    c->argc = argc;
    c->args = args;
    return c;
}

lab_parser_ast_prototype_parameter_t* lab_parser_ast_prototype_parameter_make(char* name) {
    lab_parser_ast_prototype_parameter_t* p = (lab_parser_ast_prototype_parameter_t*)malloc(sizeof(lab_parser_ast_prototype_parameter_t));
    p->name = name;
    return p;
}

lab_parser_ast_prototype_t* lab_parser_ast_prototype_make(char* name, size_t argc, lab_parser_ast_prototype_parameter_t* args) {
    lab_parser_ast_prototype_t* p = (lab_parser_ast_prototype_t*)malloc(sizeof(lab_parser_ast_prototype_t));
    p->name = name;
    p->argc = argc;
    p->args = args;
    return p;
}

lab_parser_ast_function_t* lab_parser_ast_function_make(lab_parser_ast_prototype_t* head, lab_parser_ast_expr_t* body) {
    lab_parser_ast_function_t* f = (lab_parser_ast_function_t*)malloc(sizeof(lab_parser_ast_function_t));
    f->head = head;
    f->body = body;
    return f;
}

/*
    Actual parser
*/

lab_parser_ast_expr_t* lab_parser_parse_expr(lab_parser_ast_t* ast, lab_parser_token_container_t* tokens);

int lab_parser_ast_expr_call_check(lab_parser_ast_t* ast, lab_parser_ast_expr_call_t* call) {
    for(lab_parser_ast_prototype_t* p = ast->prototypes; p != NULL; p = p->next) {
        if(!strcmp(call->callee, p->name) && call->argc == p->argc) {
            return 0;
        }
    }
    return 1;
}

lab_parser_ast_expr_t* lab_parser_parse_ident_expr(lab_parser_ast_t* ast, lab_parser_token_container_t* tokens) {

    char* identifier;
    lab_parser_ast_expr_t* id_expr;

    if(tokens->tokens->tokens[tokens->iter].id != (int)lab_tok_lparen) {
        id_expr = lab_parser_ast_expr_make(lab_parser_ast_expr_var_make(tokens->tokens->tokens[tokens->iter].data), lab_expr_var);
    } else {
        lab_parser_ast_expr_t* args = NULL;
        size_t count = 0;

        lab_lexer_token_t* begin_token = &tokens->tokens->tokens[tokens->iter];

        ++tokens->iter;
        for(;;) {
            lab_parser_ast_expr_t* e = lab_parser_parse_expr(ast, tokens);

            if(e==NULL) {
                args = NULL;
                break;
            }

            e->next = args;
            args = e;
            ++count;

            if(tokens->tokens->tokens[tokens->iter].id == (int)lab_tok_rparen) {
                break;
            }

            if(tokens->tokens->tokens[tokens->iter].id != (int)lab_tok_comma) {
                lab_errorln("Expected ')' or ',' in arguemnt list at line: %d, column: %d", tokens->tokens->tokens[tokens->iter].line, tokens->tokens->tokens[tokens->iter].column);
                break;
            }
            ++tokens->iter;
        }
        ++tokens->iter;

        lab_parser_ast_expr_call_t* call = lab_parser_ast_expr_call_make(tokens->tokens->tokens[tokens->iter].data, count, args);
        if(lab_parser_ast_expr_call_check(ast, call)==0) {
            id_expr = lab_parser_ast_expr_make(call, lab_expr_call);
        } else {
            lab_errorln("Function \"%s\" does not exist at line: %d, column: %d", begin_token->data, begin_token->line, begin_token->column);
        }
    }

    return id_expr;
}

lab_parser_ast_expr_t* lab_parser_parse_expr(lab_parser_ast_t* ast, lab_parser_token_container_t* tokens) {
    return NULL;
}

lab_parser_ast_expr_t* lab_parser_parse_expr_num(lab_parser_ast_t* ast, lab_parser_token_container_t* tokens) {
    
}