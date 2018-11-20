#include "ast.h"

#include <stdlib.h>
#include <string.h>

lab_ast_expr_t* lab_ast_expr_make(lab_mempool_t* pool, void* expr, lab_ast_expr_kind_e_t kind) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_expr_t));
    lab_ast_expr_t* e = (lab_ast_expr_t*)suballoc->data;
    e->expr = expr;
    e->kind = kind;
    e->next = NULL;
    return e;
}

lab_ast_expr_num_t* lab_ast_expr_num_make_float(lab_mempool_t* pool, float  val) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_expr_num_t));
    lab_ast_expr_num_t* n = (lab_ast_expr_num_t*)suballoc->data;
    n->is_float    = true;
    n->as.floating = val;
    return n;
}

lab_ast_expr_num_t* lab_ast_expr_num_make_int  (lab_mempool_t* pool, size_t val) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_expr_num_t));
    lab_ast_expr_num_t* n = (lab_ast_expr_num_t*)suballoc->data;
    n->is_float   = false;
    n->as.integer = val;
    return n;
}

lab_ast_expr_var_t* lab_ast_expr_var_make(lab_mempool_t* pool, const char* name, size_t name_len) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_expr_var_t) + name_len + 1);
    lab_ast_expr_var_t* v = (lab_ast_expr_var_t*)suballoc->data;
    v->name = (char*)(suballoc->data + sizeof(lab_ast_expr_var_t));
    v->name[name_len] = '\0';
    memcpy(v->name, name, name_len);
    return v;
}

lab_ast_expr_bin_t* lab_ast_expr_bin_make(lab_mempool_t* pool, char op, lab_ast_expr_t* lhs, lab_ast_expr_t* rhs) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_expr_bin_t));
    lab_ast_expr_bin_t* b = (lab_ast_expr_bin_t*)suballoc->data;
    b->op  = op;
    b->lhs = lhs;
    b->rhs = rhs;
}