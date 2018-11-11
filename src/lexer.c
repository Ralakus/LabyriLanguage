#include <lab/logger.h>
#include "lexer.h"

bool lab_lexer_ruleset_init(lab_lexer_ruleset_t* ruleset,
                            size_t rule_count) {
    return lab_vec_init(ruleset, sizeof(lab_lexer_rule_t), rule_count);
}

void lab_lexer_ruleset_free(lab_lexer_ruleset_t* ruleset) {
    lab_vec_free(ruleset);
}
bool lab_lexer_ruleset_add_rule(lab_lexer_ruleset_t*      ruleset,
                                lab_lexer_rule_callback_t rule_function,
                                lab_lexer_callback_t      callback) {
    
    if(!lab_vec_push_back(ruleset, NULL)) { return false; }
    ((lab_lexer_rule_t*)lab_vec_at(ruleset, lab_vec_size(ruleset) - 1))->rule_function     = rule_function;
    ((lab_lexer_rule_t*)lab_vec_at(ruleset, lab_vec_size(ruleset) - 1))->callback          = callback;

    return true;
}

bool lab_lexer_token_container_init(lab_lexer_token_container_t* container) {
    return lab_vec_init(container, sizeof(lab_lexer_token_t), 0);
}
void lab_lexer_token_container_free(lab_lexer_token_container_t* container) {
    lab_vec_free(container);
}

bool lab_lexer_token_container_append(lab_lexer_token_container_t* container,
                                      const lab_vec_t* code,
                                      size_t iter,
                                      int id, 
                                      char* data, 
                                      size_t line, 
                                      size_t column) {

    ++container->used_size;
    if(container->used_size > container->alloc_size) {
        if(!lab_vec_resize(container, (
            container->used_size + (code->used_size / iter + (code->used_size % iter != 0))
        ))) {
            lab_errorln("Failed to size token container after %d tokens!", container->used_size);
            return false;
        }
    }

    lab_lexer_token_t* tok = lab_vec_at(container, container->used_size - 1);
    tok->id     = id;
    tok->data   = data;
    tok->line   = line;
    tok->column = column;

    return true;

}

bool lab_lexer_lex(lab_lexer_token_container_t* tokens, 
                   lab_lexer_ruleset_t* ruleset, 
                   const lab_vec_t* code, 
                   void* user_data) {

    lab_lexer_iterator_t pos;
    pos.iter   = 0;
    pos.line   = 1;
    pos.column = 1;

    for(; pos.iter < code->used_size; lab_lexer_iter_next(code, &pos)) {

        char cur_char = ((char*)code->raw_data)[pos.iter];
        bool found_callback = false;

        for(size_t i = 0; i < lab_vec_size(ruleset); i++) {

            lab_lexer_rule_t* rule = (lab_lexer_rule_t*)lab_vec_at(ruleset, i);

            if(rule->rule_function(cur_char)) {
                if(!rule->callback(code, &pos, tokens, user_data)) {
                    lab_errorln("Failed to run callback for character: \'%c\' at line: %d, column: %d", cur_char, pos.line, pos.column);
                    return false;
                }
                found_callback = true;
                break;
            }

        }

        if(cur_char=='\0') {
            break;
        } else if(!found_callback) {
            lab_errorln("Unexpected character: \"%c\" at line: %d, column: %d", cur_char, pos.line, pos.column);
        }

    }

    return true;

}

void lab_lexer_iter_next(const lab_vec_t* code, 
                         lab_lexer_iterator_t* iter) {

    ++iter->iter;

    if(((char*)code->raw_data)[iter->iter] == '\n') {

        ++iter->line;
          iter->column = 1;

    } else {

        ++iter->column;

    }

}

void lab_lexer_iter_prev(const lab_vec_t* code, 
                         lab_lexer_iterator_t* iter) {

    --iter->iter;

    if(((char*)code->raw_data)[iter->iter] == '\n') {

        --iter->line;
          iter->column = 1;
        for(size_t i = iter->iter; i > 0; i--) {
            if(((char*)code->raw_data)[i] == '\n') {
                break;
            } else {
                ++iter->column;
            }
        }

    } else {

        --iter->column;

    }

}