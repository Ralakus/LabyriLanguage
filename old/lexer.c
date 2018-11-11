#include <lab/logger.h>
#include "lexer.h"
#include <lab/helper_macros.h>

#include <stdlib.h>
#include <string.h>

#define NOT_SO_FAST_CEIL(x) ((float)(long)((x)+1))

int lab_token_container_append(lab_lexer_token_container_t* container, lab_lexer_token_t token, const lab_lexer_iterator_t* pos, size_t max_code_len) {

    ++container->count;
    if(container->count > container->alloc_count) {
        container->alloc_count = container->count + ( max_code_len / pos->iter + (max_code_len % pos->iter != 0));
        /*(size_t)NOT_SO_FAST_CEIL(container->count *
        (((float)max_code_len / (float)pos->iter == 0 ? 1 : (float)pos->iter) > 1.0f ? ((float)max_code_len / (float)pos->iter == 0 ? 1 : (float)pos->iter) : 1.0f));*/
        
        container->tokens = (lab_lexer_token_t*)realloc(container->tokens, sizeof(lab_lexer_token_t) * container->alloc_count);

        if(container->tokens == NULL) {
            lab_errorln("Failed to reallocate and add rule!");
            return 1;
        }
    }

    memcpy(&container->tokens[container->count-1], &token, sizeof(lab_lexer_token_t));

    return 0;
}

int lab_lexer_rules_init(lab_lexer_rules_t* rules) {
    if(rules==NULL) {
        return 1;
    } else {
        rules->count = 0;
        rules->rules = NULL;
        return 0;
    }
}

int lab_lexer_rules_free(lab_lexer_rules_t* rules) {
    if(rules->rules == NULL) {
        return 0;
    } else {
        free(rules->rules);
        rules->rules = NULL;
        rules->count = 0;
        return 0;
    }
}

int lab_lexer_token_container_init(lab_lexer_token_container_t* container) {
    container->alloc_count = 0;
    container->count  = 0;
    container->tokens = NULL;
    return 0;
}
int lab_lexer_token_container_free(lab_lexer_token_container_t* container) {
    if(container->tokens==NULL) {
    } else {
        for(size_t i = 0; i < container->count; i++) {
            free(container->tokens[i].data);
        }
        free(container->tokens);
    }
    container->alloc_count = 0;
    container->count = 0;
    return 0;
}

lab_lexer_token_t lab_lexer_token_make(int id, char* data, lab_lexer_iterator_t* pos) {
    lab_lexer_token_t token;
    token.id     = id;
    token.data   = data;
    token.line   = pos->line;
    token.column = pos->column;
    return token;
}

int lab_lexer_add_rule(lab_lexer_rules_t* rules, const char* rule, lab_lexer_callback_t callback) {

    ++rules->count;

    if(rules->rules == NULL) {
        rules->rules = (lab_lexer_rule_t*)malloc(sizeof(lab_lexer_rule_t) * rules->count);
    } else {
        rules->rules = (lab_lexer_rule_t*)realloc(rules->rules, sizeof(lab_lexer_rule_t) * rules->count);
    }

    if(rules->rules == NULL) {
        lab_errorln("Failed to reallocate and add rule!");
        return 1;
    }

    rules->rules[rules->count-1].callback = callback;
    rules->rules[rules->count-1].rule     = rule;

    return 0;
}

int lab_lexer_lex(lab_lexer_token_container_t* tokens, const char* code, size_t code_len, const lab_lexer_rules_t* rules, void* user_data) {
    if(code_len == 0) {
        code_len = strlen(code);
    }

    lab_lexer_iterator_t pos;
    pos.iter   = 0;
    pos.line   = 1;
    pos.column = 0;

    for (pos.iter = 0; pos.iter < code_len; lab_lexer_iter_next(code, &pos)) {
        
        char cur_char = code[pos.iter];
        int found_callback = 0;

        for(size_t j = 0; j < rules->count; j++) {

            for(size_t k = 0;; k++) {
                if(rules->rules[j].rule[k]=='\0') {
                    break;
                }

                if(cur_char==rules->rules[j].rule[k]) {
                    lab_lexer_token_t temp_tok = rules->rules[j].callback(code, &pos, code_len, user_data);
                    if(temp_tok.id >= 0) {
                        lab_token_container_append(tokens, temp_tok, &pos, code_len);
                    }
                    found_callback = 1;
                    break;
                }

            }

            if(found_callback==1) {
                break;
            }

        }

        if(found_callback==0) {
            lab_errorln("Unexpected character: \"%c\" at line: %d, column: %d", cur_char, pos.line, pos.column);
        }
    }
    return 0;
}

void lab_lexer_iter_next(const char* code, lab_lexer_iterator_t* iter) {
    ++iter->iter;

    if(code[iter->iter]=='\n') {

        ++iter->line;
          iter->column = 0;

    } /*else if(code[iter->iter] == '\0') { // Removed since it's not used

        return 1;

    } */else{

        ++iter->column;

    }
    return;
}