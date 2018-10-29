#include "logger.h"
#include "lexer.h"

#include <stdlib.h>
#include <string.h>

typedef struct _lab_lexer_rule_t {
    const char*          rule;
    lab_lexer_callback_t callback;
} _lab_lexer_rule_t;

struct lab_lexer_rules_t {
    int count;
    _lab_lexer_rule_t* rules;
};

static int _tok_append(lab_lexer_token_container_t* container, lab_lexer_token_t token) {

    ++container->count;

    if(container->tokens == NULL) {
        container->tokens = malloc(sizeof(_lab_lexer_rule_t) * container->count);
    } else {
        container->tokens = realloc(container->tokens, sizeof(_lab_lexer_rule_t) * container->count);
    }

    if(container->tokens == NULL) {
        lab_errorln("Failed to reallocate and add rule!");
        return 1;
    }

    memcpy(&container->tokens[container->count-1], &token, sizeof(lab_lexer_token_t));

    return 0;
}

lab_lexer_rules_t* lab_lexer_rules_new() {
    lab_lexer_rules_t* rules = malloc(sizeof(lab_lexer_rules_t));
    if(rules==NULL) {
        return NULL;
    } else {
        rules->count = 0;
        rules->rules = NULL;
        return rules;
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
    container->count = 0;
    return 0;
}

lab_lexer_token_t lab_lexer_token_make(int id, char* data) {
    lab_lexer_token_t token;
    token.id = id;
    token.data = data;
    return token;
}

int lab_lexer_add_rule(lab_lexer_rules_t* rules, const char* rule, lab_lexer_callback_t callback) {

    ++rules->count;

    if(rules->rules == NULL) {
        rules->rules = malloc(sizeof(_lab_lexer_rule_t) * rules->count);
    } else {
        rules->rules = realloc(rules->rules, sizeof(_lab_lexer_rule_t) * rules->count);
    }

    if(rules->rules == NULL) {
        lab_errorln("Failed to reallocate and add rule!");
        return 1;
    }

    rules->rules[rules->count-1].callback = callback;
    rules->rules[rules->count-1].rule     = rule;

    return 0;
}

int lab_lexer_lex(lab_lexer_token_container_t* tokens, const char* code, const lab_lexer_rules_t* rules, void* user_data) {
    size_t code_len = strlen(code);
    size_t line   = 1;
    size_t column = 0;

    for (size_t i = 0; i < code_len; i++) {
        
        char cur_char = code[i];
        int found_callback = 0;

        if(cur_char=='\n') {
            ++line;
            column = 0;
        } else {
            ++column;
        }

        for(size_t j = 0; j < rules->count; j++) {

            size_t rule_j_len = strlen(rules->rules[j].rule);
            for(size_t k = 0; k < rule_j_len; k++) {

                if(cur_char==rules->rules[j].rule[k]) {
                    _tok_append(tokens, rules->rules[j].callback(code, &i, code_len, user_data));
                    found_callback = 1;
                    break;
                }

            }

            if(found_callback==1) {
                break;
            }

        }

        if(found_callback==0) {
            lab_errorln("Unexpected character: \"%c\" at line: %d, column: %d", cur_char, line, column);
        }
    }
    return 0;
}