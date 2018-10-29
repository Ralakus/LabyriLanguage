#include "logger.h"
#include "lexer.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum tokens_e {
    tok_nil,
    tok_whitespace,
    tok_identifier,
    tok_number,

    tok_lparen,
    tok_rparen,
    tok_lbracket,
    tok_rbracket,
    tok_lcurley,
    tok_rcurley,

    tok_comma,
    tok_colon,
    tok_semicolon,

    tok_func,
    tok_let,

    tok_equals,
} tokens_e;

#define TOK_TO_STRING_TEMPLATE(token, string) case token: {                                                                 \
                                                  buffer = malloc(strlen(string)+1);                                        \
                                                  if(buffer==NULL) {                                                        \
                                                      lab_errorln("Failed to allocate buffer for function "#string"!");     \
                                                      return NULL;                                                          \
                                                  }                                                                         \
                                                  buffer[strlen(string)] = '\0';                                            \
                                                  strcpy(buffer, string);                                                   \
                                                  break;                                                                    \
                                              }

char* tok_to_string(tokens_e tok) {
    char* buffer = NULL;
    switch(tok) {
        TOK_TO_STRING_TEMPLATE(tok_nil, "nil")
        TOK_TO_STRING_TEMPLATE(tok_whitespace, "whitespace")
        TOK_TO_STRING_TEMPLATE(tok_identifier, "identifier")
        TOK_TO_STRING_TEMPLATE(tok_number, "number")
        TOK_TO_STRING_TEMPLATE(tok_lparen, "left paren")
        TOK_TO_STRING_TEMPLATE(tok_rparen, "right paren")
        TOK_TO_STRING_TEMPLATE(tok_lbracket, "left bracket")
        TOK_TO_STRING_TEMPLATE(tok_rbracket, "right bracket")
        TOK_TO_STRING_TEMPLATE(tok_lcurley, "left curley")
        TOK_TO_STRING_TEMPLATE(tok_rcurley, "right curley")
        TOK_TO_STRING_TEMPLATE(tok_comma, "comma")
        TOK_TO_STRING_TEMPLATE(tok_colon, "colon")
        TOK_TO_STRING_TEMPLATE(tok_semicolon, "semicolon")
        TOK_TO_STRING_TEMPLATE(tok_func, "function")
        TOK_TO_STRING_TEMPLATE(tok_let, "let")
        TOK_TO_STRING_TEMPLATE(tok_equals, "equals")
        default: {
            break;
        }
    }
    return buffer;
}

lab_lexer_token_t alpha_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    size_t bufpos = 0;
    static const char* reserved[]    = { "Func", "let" };
    static tokens_e reserved_types[] = { tok_func, tok_let };
    for(;(*iter) < max_len; (*iter)++) {
        buffer[bufpos] = code[*iter];
        ++bufpos;
        if(bufpos >= sizeof(buffer)) {
            lab_errorln("Identifier exceeds 128 char limit!");
            return lab_lexer_token_make((int)tok_nil, NULL);
        }
        if(bufpos % 2 == 0) {
            for(size_t i = 0; i < (sizeof(reserved) / sizeof(const char*)); i++) {
                if(buffer[0] != '\0') {
                    if(strcmp(buffer, reserved[i])==0 && (!isalpha(code[(*iter) + 1]) && !isdigit(code[(*iter) + 1]))) {
                        return lab_lexer_token_make((int)reserved_types[i], NULL);
                    }
                }
            }
        }
        if(!isalpha(code[(*iter) + 1]) && !isdigit(code[(*iter) + 1])) {

            if(bufpos > 0) {
                char* ident = calloc(bufpos, sizeof(char));
                if(ident==NULL) {
                    lab_errorln("Failed to allocate buffer for identifier token!");
                } else {
                    memcpy(ident, buffer, bufpos);
                }
                return lab_lexer_token_make((int)tok_identifier, ident);
            }
            break;
        }
    }
    return lab_lexer_token_make((int)tok_nil, NULL);
}

lab_lexer_token_t whitespace_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    char type = code[*iter];
    char* type_name = NULL;
    switch (type) {
        case ' ': {
            type_name = malloc(sizeof("space"));
            if(type_name==NULL) {
                lab_errorln("Failed to allocate buffer for whitespace type: space");
            } else {
                type_name[sizeof("space")-1] = '\0';
                memcpy(type_name, "space", sizeof("space"));
            }
            break;
        } case '\t': {
            type_name = malloc(sizeof("tab"));
            if(type_name==NULL) {
                lab_errorln("Failed to allocate buffer for whitespace type: tab");
            } else {
                type_name[sizeof("tab")-1] = '\0';
                memcpy(type_name, "tab", sizeof("tab"));
            }
            break;
        } case '\n': {
            type_name = malloc(sizeof("newline"));
            if(type_name==NULL) {
                lab_errorln("Failed to allocate buffer for whitespace type: newline");
            } else {
                type_name[sizeof("newline")-1] = '\0';
                memcpy(type_name, "newline", sizeof("newline"));
            }
            break;
        }
        default: break;
    }
    return lab_lexer_token_make((int)tok_whitespace, type_name);
}

lab_lexer_token_t numeric_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));
    size_t bufpos = 0;
    for(;(*iter) < max_len; (*iter)++ ) {
        buffer[bufpos] = code[*iter];
        ++bufpos;
        if(bufpos >= sizeof(buffer)) {
            lab_errorln("Number exceeds 128 char limit!");
            return lab_lexer_token_make((int)tok_nil, NULL);
        }
        if(!isdigit(code[(*iter) + 1]) && code[(*iter) + 1] != '.') {
            if(bufpos > 0) {
                char* num = calloc(bufpos, sizeof(char));
                if(num==NULL) {
                    lab_errorln("Failed to allocate buffer for numerical token!");
                } else {
                    memcpy(num, buffer, bufpos);
                }
                return lab_lexer_token_make((int)tok_number, num);
            }
        }
    }
    return lab_lexer_token_make((int)tok_nil, NULL);
}

lab_lexer_token_t symbol_callback(const char* code, size_t* iter, size_t max_len, void* user_data) {
    switch(code[*iter]) {
        case '(': return lab_lexer_token_make((int)tok_lparen, NULL);
        case ')': return lab_lexer_token_make((int)tok_rparen, NULL);
        case '[': return lab_lexer_token_make((int)tok_lbracket, NULL);
        case ']': return lab_lexer_token_make((int)tok_rbracket, NULL);
        case '{': return lab_lexer_token_make((int)tok_lcurley, NULL);
        case '}': return lab_lexer_token_make((int)tok_rcurley, NULL);
        case ',': return lab_lexer_token_make((int)tok_comma, NULL);
        case ':': return lab_lexer_token_make((int)tok_colon, NULL);
        case ';': return lab_lexer_token_make((int)tok_semicolon, NULL);
        case '=': return lab_lexer_token_make((int)tok_equals, NULL);
        default:  return lab_lexer_token_make((int)tok_nil, NULL);
    }
}



int main(int argc, char* argv[]) {

    size_t file_count = argc - 1;
    char** file_names = NULL;
    char** file_contents = NULL;

    if(argc > 1) {
        for(int i = 1; i < argc; i++) {
            
        }
    }
    
    lab_lexer_rules_t* rules = lab_lexer_rules_new();
    lab_lexer_token_container_t tokens;
    lab_lexer_token_container_init(&tokens);

    lab_lexer_add_rule(rules, "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm", alpha_callback);
    lab_lexer_add_rule(rules, " \n\t", whitespace_callback);
    lab_lexer_add_rule(rules, "1234567890.", numeric_callback);
    lab_lexer_add_rule(rules, "()[]{},:;=", symbol_callback);

    lab_lexer_lex(&tokens, "Func Noice123(It, cool) {\n\tlet x = 1.5;\n}", rules, NULL);

    for(size_t i = 0; i < tokens.count; i++) {
        char* tok_str = tok_to_string((tokens_e)tokens.tokens[i].id);
        lab_successln("Token: %s: %s", tok_str, tokens.tokens[i].data);
        free(tok_str);
    }

    lab_lexer_token_container_free(&tokens);

    lab_lexer_rules_free(rules);

    return 0;
}