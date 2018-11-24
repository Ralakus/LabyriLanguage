#include "parser.h"

#define NEXT_TOKEN() ((lab_lexer_token_t*)lab_vec_at(container->tokens, ++container->cur))
#define CUR_TOKEN    ((lab_lexer_token_t*)lab_vec_at(container->tokens, container->cur))

bool lab_parser_token_container_init(lab_parser_token_container_t* container, lab_lexer_token_container_t* tokens) {
    if(tokens != NULL) {
        container->tokens = tokens;
        container->cur = 0;
        return true;
    } else {
        container->tokens = NULL;
        container->cur = 0;
        return false;
    }
}