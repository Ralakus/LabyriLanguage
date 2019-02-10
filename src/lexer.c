#include <string.h>
#include "lexer.h"

bool lab_lexer_token_container_init(lab_lexer_token_container_t* container) {
    if(!lab_vec_init(&container->tokens, sizeof(lab_lexer_token_t), 0)) {
        return false;
    } else {
        container->code = NULL;
        return true;
    }
}

void lab_lexer_token_container_free(lab_lexer_token_container_t* container) {
    lab_vec_free(&container->tokens);
}

bool lab_lexer_token_container_append(lab_lexer_token_container_t* container,
                                      size_t code_size,
                                      size_t iter,
                                      lab_tokens_e_t type,
                                      const char* data,
                                      size_t data_len,
                                      size_t line,
                                      size_t column) {

    ++container->tokens.used_size;
    if(container->tokens.used_size > container->tokens.alloc_size) {
        if(!lab_vec_resize(&container->tokens, (
            container->tokens.used_size + (code_size / iter + (code_size % iter != 0))
        ))) {
            lab_errorln("Failed to resize token container after %d tokens!", container->tokens.used_size);
            return false;
        }
    }

    lab_lexer_token_t* tok = (lab_lexer_token_t*)lab_vec_at(&container->tokens, container->tokens.used_size - 1);
    tok->type     = type;
    tok->data     = data;
    tok->data_len = data_len;
    tok->line     = line;
    tok->column   = column;

    return true;

}

bool lab_lexer_lex(lab_lexer_token_container_t* container, const char* code) {
    size_t code_len = strlen(code);

    
    

}