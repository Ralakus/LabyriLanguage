#include "ast.h"

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

lab_ast_expr_var_t* lab_ast_expr_var_make(lab_mempool_t* pool, const char* name) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_expr_var_t));
    lab_ast_expr_var_t* v = (lab_ast_expr_var_t*)suballoc->data;
    v->name = name;
    return v;
}

lab_ast_expr_bin_t* lab_ast_expr_bin_make(lab_mempool_t* pool, char op, lab_ast_expr_t* lhs, lab_ast_expr_t* rhs) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_expr_bin_t));
    lab_ast_expr_bin_t* b = (lab_ast_expr_bin_t*)suballoc->data;
    b->op  = op;
    b->lhs = lhs;
    b->rhs = rhs;
    return b;
}

lab_ast_expr_call_t* lab_ast_expr_call_make(lab_mempool_t* pool, char* callee, lab_ast_expr_t* args, size_t argc) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_expr_call_t));
    lab_ast_expr_call_t* c = (lab_ast_expr_call_t*)suballoc->data;
    c->callee = callee;
    c->argc = argc;
    c->args = args;
    return c;
}

lab_ast_parameter_t* lab_ast_parameter_make(lab_mempool_t* pool, char* name) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_parameter_t));
    lab_ast_parameter_t* p = (lab_ast_parameter_t*)suballoc->data;
    p->name = name;
    return p;
}

lab_ast_prototype_t* lab_ast_prototype_make(lab_mempool_t* pool, lab_ast_parameter_t* args, size_t argc) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_prototype_t));
    lab_ast_prototype_t* p = (lab_ast_prototype_t*)suballoc->data;
    p->args = args;
    p->argc = argc;
    return p;
}

lab_ast_function_t* lab_ast_function_make(lab_mempool_t* pool, lab_ast_prototype_t* head, lab_ast_expr_t* body) {
    lab_mempool_suballoc_t* suballoc = lab_mempool_suballoc_alloc(pool, sizeof(lab_ast_function_t));
    lab_ast_function_t* f = (lab_ast_function_t*)suballoc->data;
    f->head = head;
    f->body = body;
    return f;
}