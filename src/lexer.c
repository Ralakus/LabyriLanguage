#include "lexer.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

#define TOK_TO_STRING_TEMPLATE(token, string) case token: {                                                                 \
                                                  buffer = (char*)malloc(strlen(string)+1);                                 \
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
        TOK_TO_STRING_TEMPLATE(tok_whitespace_space, "whitespace space")
        TOK_TO_STRING_TEMPLATE(tok_whitespace_tab, "whitespace tab")
        TOK_TO_STRING_TEMPLATE(tok_whitespace_newline, "whitespace newline")
        TOK_TO_STRING_TEMPLATE(tok_whitespace_return, "whitespace return")
        TOK_TO_STRING_TEMPLATE(tok_identifier, "identifier")
        TOK_TO_STRING_TEMPLATE(tok_number, "number")
        TOK_TO_STRING_TEMPLATE(tok_char, "char")
        TOK_TO_STRING_TEMPLATE(tok_string, "string")
        TOK_TO_STRING_TEMPLATE(tok_lparen, "left paren")
        TOK_TO_STRING_TEMPLATE(tok_rparen, "right paren")
        TOK_TO_STRING_TEMPLATE(tok_lbracket, "left bracket")
        TOK_TO_STRING_TEMPLATE(tok_rbracket, "right bracket")
        TOK_TO_STRING_TEMPLATE(tok_lcurley, "left curley")
        TOK_TO_STRING_TEMPLATE(tok_rcurley, "right curley")
        TOK_TO_STRING_TEMPLATE(tok_comma, "comma")
        TOK_TO_STRING_TEMPLATE(tok_colon, "colon")
        TOK_TO_STRING_TEMPLATE(tok_semicolon, "semicolon")
        TOK_TO_STRING_TEMPLATE(tok_kw_func, "function")
        TOK_TO_STRING_TEMPLATE(tok_kw_let, "let")
        TOK_TO_STRING_TEMPLATE(tok_kw_return, "return")
        TOK_TO_STRING_TEMPLATE(tok_operator_plus, "operator plus")
        TOK_TO_STRING_TEMPLATE(tok_operator_minus, "operator minus")
        TOK_TO_STRING_TEMPLATE(tok_operator_mul, "operator multiply")
        TOK_TO_STRING_TEMPLATE(tok_operator_div, "operator divide")
        TOK_TO_STRING_TEMPLATE(tok_operator_equals, "operator equals")
        TOK_TO_STRING_TEMPLATE(tok_operator_xor, "operator xor")
        TOK_TO_STRING_TEMPLATE(tok_operator_and, "operator and")
        TOK_TO_STRING_TEMPLATE(tok_operator_lesst, "operator less than")
        TOK_TO_STRING_TEMPLATE(tok_operator_greatert, "operator greater than")
        TOK_TO_STRING_TEMPLATE(tok_operator_or, "operator or")
        TOK_TO_STRING_TEMPLATE(tok_operator_not, "operator not")
        default: {
            break;
        }
    }
    return buffer;
}

lab_lexer_token_t alpha_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    lab_lexer_iterator_t begin_iter = *iter;

    static const char* reserved[]    = {       "Func",      "let",      "return" };
    static tokens_e reserved_types[] = { tok_kw_func, tok_kw_let, tok_kw_return };

    for(;iter->iter < max_len; lab_lexer_iter_next(code, iter) ) {

        if(!isalpha(code[iter->iter + 1]) && !isdigit(code[iter->iter + 1])) {

            for(size_t i = 0; i < (sizeof(reserved) / sizeof(const char*)); i++) {

                for(size_t j = 0;; j++) {

                    if(j >= iter->iter - (begin_iter.iter + 1)) {

                        return lab_lexer_token_make((int)reserved_types[i], NULL);

                    } else if(reserved[i][j]=='\0') {
                        break;
                    }
                    if(reserved[i][j]==(code + begin_iter.iter)[j]) {
                        continue;
                    } else {
                        break;
                    }
                }
            }

            char* ident = (char*)malloc((iter->iter - begin_iter.iter) + 2);
            if(ident==NULL) {

                lab_errorln("Failed to allocate buffer for identifier token for identifier at line: %d, column: %d!", begin_iter.line, begin_iter.column);

            } else {

                ident[(iter->iter - begin_iter.iter) + 1] = '\0';
                memcpy(ident, code + begin_iter.iter, (iter->iter - begin_iter.iter) + 1);

            }

            return lab_lexer_token_make((int)tok_identifier, ident);
        }
    }
    return lab_lexer_token_make((int)tok_nil, NULL);
}

lab_lexer_token_t whitespace_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    char type = code[iter->iter];
    char* type_name = NULL;
    switch (type) {
        case ' ' : return lab_lexer_token_make((int)tok_whitespace_space, NULL);
        case '\t': return lab_lexer_token_make((int)tok_whitespace_tab, NULL);
        case '\n': return lab_lexer_token_make((int)tok_whitespace_newline, NULL);
        case '\r': return lab_lexer_token_make((int)tok_whitespace_return, NULL);
        default  : return lab_lexer_token_make((int)tok_nil, type_name);;
    }
}

lab_lexer_token_t numeric_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    lab_lexer_iterator_t begin_iter = *iter;

    for(;iter->iter < max_len; lab_lexer_iter_next(code, iter) ) {

        if(!isdigit(code[iter->iter + 1]) && code[iter->iter + 1] != '.') {

            char* num = (char*)malloc((iter->iter - begin_iter.iter) + 2);
            if(num==NULL) {

                lab_errorln("Failed to allocate buffer for numerical token for number at line: %d, column: %d!", iter->line, iter->column);

            } else {

                num[(iter->iter - begin_iter.iter) + 1] = '\0';
                memcpy(num, code + begin_iter.iter, (iter->iter - begin_iter.iter) + 1);

            }

            return lab_lexer_token_make((int)tok_number, num);
        }
    }
    return lab_lexer_token_make((int)tok_nil, NULL);
}

lab_lexer_token_t symbol_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    switch(code[iter->iter]) {
        case '(': return lab_lexer_token_make((int)tok_lparen,    NULL);
        case ')': return lab_lexer_token_make((int)tok_rparen,    NULL);
        case '[': return lab_lexer_token_make((int)tok_lbracket,  NULL);
        case ']': return lab_lexer_token_make((int)tok_rbracket,  NULL);
        case '{': return lab_lexer_token_make((int)tok_lcurley,   NULL);
        case '}': return lab_lexer_token_make((int)tok_rcurley,   NULL);
        case ',': return lab_lexer_token_make((int)tok_comma,     NULL);
        case ':': return lab_lexer_token_make((int)tok_colon,     NULL);
        case ';': return lab_lexer_token_make((int)tok_semicolon, NULL);
        default:  return lab_lexer_token_make((int)tok_nil,       NULL);
    }
}

lab_lexer_token_t operator_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    switch(code[iter->iter]) {
        case '+': return lab_lexer_token_make((int)tok_operator_plus,         NULL);
        case '-': return lab_lexer_token_make((int)tok_operator_minus,        NULL);
        case '*': return lab_lexer_token_make((int)tok_operator_mul,          NULL);
        case '/': return lab_lexer_token_make((int)tok_operator_div,          NULL);
        case '=': return lab_lexer_token_make((int)tok_operator_equals,       NULL);
        case '^': return lab_lexer_token_make((int)tok_operator_xor,          NULL);
        case '<': return lab_lexer_token_make((int)tok_operator_lesst,        NULL);
        case '>': return lab_lexer_token_make((int)tok_operator_greatert,     NULL);
        case '|': return lab_lexer_token_make((int)tok_operator_or,           NULL);
        case '!': return lab_lexer_token_make((int)tok_operator_not,          NULL);
        default:  return lab_lexer_token_make((int)tok_nil,                   NULL);
    }
}

lab_lexer_token_t string_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    int mode = code[iter->iter]=='\"' ? 1 : -1; // 1 means it's lexing a string, -1 means char
    size_t begin_index = iter->iter;
    size_t end_index = 0;
    lab_lexer_iter_next(code, iter);
    lab_lexer_iterator_t begin_pos = *iter;
    for(;iter->iter < max_len; lab_lexer_iter_next(code, iter) ) {
        if(code[iter->iter]=='\"' && mode == 1) {
            end_index = iter->iter - 1;
            break;
        } else if(code[iter->iter]=='\'' && mode == -1) {
            end_index = iter->iter -1;
            break;
        }
    }
    if(end_index==0) {
        lab_errorln("Failed to find string closing statement for string starting at line: %d, column: %d", begin_pos.line, begin_pos.column);
        return lab_lexer_token_make((int)tok_nil, NULL);
    } else {
        char* buffer = (char*)malloc((end_index - begin_index) + 1);
        if(buffer==NULL) {
            lab_errorln("Failed to allocate string buffer for string starting at line: %d, column: %d", begin_pos.line, begin_pos.column);
            return lab_lexer_token_make((int)tok_nil, NULL);
        }
        buffer[end_index - begin_index] = '\0';
        memcpy(buffer, code + begin_index + 1, end_index - begin_index);
        return lab_lexer_token_make(mode == 1 ? (int)tok_string : (int)tok_char, buffer);
    }
    return lab_lexer_token_make((int)tok_nil, NULL);
}

lab_lexer_token_t eof_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    return lab_lexer_token_make((int)tok_eof, NULL);
}

int custom_lexer_lex(lab_lexer_token_container_t* tokens, const char* code, size_t code_len, void* user_data) {
    if(code_len == 0) {
        code_len = strlen(code);
    }

    lab_lexer_iterator_t pos;
    pos.iter   = 0;
    pos.line   = 1;
    pos.column = 0;

    for (pos.iter = 0; pos.iter < code_len; lab_lexer_iter_next(code, &pos)) {
        
        char cur_char = code[pos.iter];

        if(isalpha(cur_char)) {
            lab_token_container_append(tokens, alpha_callback(code, &pos, code_len, user_data), &pos, code_len);
        } else if (isdigit(cur_char) || cur_char == '.') {
           lab_token_container_append(tokens, numeric_callback(code, &pos, code_len, user_data), &pos, code_len);
        } else if(isspace(cur_char)) {
            lab_token_container_append(tokens, whitespace_callback(code, &pos, code_len, user_data), &pos, code_len);
        } else if (cur_char=='(' || cur_char==')' || cur_char=='[' || cur_char==']' || cur_char=='{' || cur_char=='}' ||
                   cur_char==',' || cur_char==':' || cur_char==';') {
            lab_token_container_append(tokens, symbol_callback(code, &pos, code_len, user_data), &pos, code_len);
        } else if (cur_char=='+' || cur_char=='-' || cur_char=='*' || cur_char=='/' || cur_char=='=' || cur_char=='^' ||
                   cur_char=='&' || cur_char=='<' || cur_char=='>' || cur_char=='|') {
            lab_token_container_append(tokens, operator_callback(code, &pos, code_len, user_data), &pos, code_len);
        } else if(cur_char=='\"' || cur_char=='\'') {
            lab_token_container_append(tokens, string_callback(code, &pos, code_len, user_data), &pos, code_len);
        }

    }
    return 0;
}