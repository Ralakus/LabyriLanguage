#include "lexer_conf.h"

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

char* tok_to_string(lab_tokens_e tok) {
    char* buffer = NULL;
    switch(tok) {
        TOK_TO_STRING_TEMPLATE(lab_tok_nil, "nil")
        TOK_TO_STRING_TEMPLATE(lab_tok_whitespace_space, "whitespace space")
        TOK_TO_STRING_TEMPLATE(lab_tok_whitespace_tab, "whitespace tab")
        TOK_TO_STRING_TEMPLATE(lab_tok_whitespace_newline, "whitespace newline")
        TOK_TO_STRING_TEMPLATE(lab_tok_whitespace_return, "whitespace return")
        TOK_TO_STRING_TEMPLATE(lab_tok_identifier, "identifier")
        TOK_TO_STRING_TEMPLATE(lab_tok_number, "number")
        TOK_TO_STRING_TEMPLATE(lab_tok_char, "char")
        TOK_TO_STRING_TEMPLATE(lab_tok_string, "string")
        TOK_TO_STRING_TEMPLATE(lab_tok_lparen, "left paren")
        TOK_TO_STRING_TEMPLATE(lab_tok_rparen, "right paren")
        TOK_TO_STRING_TEMPLATE(lab_tok_lbracket, "left bracket")
        TOK_TO_STRING_TEMPLATE(lab_tok_rbracket, "right bracket")
        TOK_TO_STRING_TEMPLATE(lab_tok_lcurley, "left curley")
        TOK_TO_STRING_TEMPLATE(lab_tok_rcurley, "right curley")
        TOK_TO_STRING_TEMPLATE(lab_tok_comma, "comma")
        TOK_TO_STRING_TEMPLATE(lab_tok_colon, "colon")
        TOK_TO_STRING_TEMPLATE(lab_tok_semicolon, "semicolon")
        TOK_TO_STRING_TEMPLATE(lab_tok_comment, "comment")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_func, "function")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_let, "let")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_return, "return")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_plus, "operator plus")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_minus, "operator minus")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_mul, "operator multiply")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_div, "operator divide")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_equals, "operator equals")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_xor, "operator xor")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_and, "operator and")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_lesst, "operator less than")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_greatert, "operator greater than")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_or, "operator or")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_not, "operator not")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_bitshiftl, "left bitshift")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_bitshiftr, "right bitshift")
        TOK_TO_STRING_TEMPLATE(lab_tok_eof, "end of file")
        default: {
            break;
        }
    }
    return buffer;
}

lab_lexer_token_t alpha_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    lab_lexer_iterator_t begin_iter = *iter;

    static const char* reserved[]    = {       "Func",      "let",      "return", "reg" };
    static lab_tokens_e reserved_types[] = { lab_tok_kw_func, lab_tok_kw_let, lab_tok_kw_return, lab_tok_eof };

    for(;iter->iter < max_len; lab_lexer_iter_next(code, iter) ) {

        if(!isalpha(code[iter->iter + 1]) && !isdigit(code[iter->iter + 1])) {

            for(size_t i = 0; i < (sizeof(reserved) / sizeof(const char*)); i++) {

                for(size_t j = 0;; j++) {

                    if(j > iter->iter - (begin_iter.iter + 1) && reserved[i][j]==(code + begin_iter.iter)[j]) {

                        return lab_lexer_token_make((int)reserved_types[i], NULL, &begin_iter);

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

            return lab_lexer_token_make((int)lab_tok_identifier, ident, &begin_iter);
        }
    }
    return lab_lexer_token_make((int)lab_tok_nil, NULL, &begin_iter);
}

lab_lexer_token_t whitespace_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    char type = code[iter->iter];
    switch (type) {
        case ' ' : return lab_lexer_token_make((int)lab_tok_whitespace_space,   NULL, iter);
        case '\t': return lab_lexer_token_make((int)lab_tok_whitespace_tab,     NULL, iter);
        case '\n': return lab_lexer_token_make((int)lab_tok_whitespace_newline, NULL, iter);
        case '\r': return lab_lexer_token_make((int)lab_tok_whitespace_return,  NULL, iter);
        default  : return lab_lexer_token_make((int)lab_tok_nil,                NULL, iter);
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

            return lab_lexer_token_make((int)lab_tok_number, num, &begin_iter);
        }
    }
    return lab_lexer_token_make((int)lab_tok_nil, NULL, &begin_iter);
}

lab_lexer_token_t symbol_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    switch(code[iter->iter]) {
        case '(': return lab_lexer_token_make((int)lab_tok_lparen,    NULL, iter);
        case ')': return lab_lexer_token_make((int)lab_tok_rparen,    NULL, iter);
        case '[': return lab_lexer_token_make((int)lab_tok_lbracket,  NULL, iter);
        case ']': return lab_lexer_token_make((int)lab_tok_rbracket,  NULL, iter);
        case '{': return lab_lexer_token_make((int)lab_tok_lcurley,   NULL, iter);
        case '}': return lab_lexer_token_make((int)lab_tok_rcurley,   NULL, iter);
        case ',': return lab_lexer_token_make((int)lab_tok_comma,     NULL, iter);
        case ':': return lab_lexer_token_make((int)lab_tok_colon,     NULL, iter);
        case ';': return lab_lexer_token_make((int)lab_tok_semicolon, NULL, iter);
        default:  return lab_lexer_token_make((int)lab_tok_nil,       NULL, iter);
    }
}

lab_lexer_token_t operator_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    switch(code[iter->iter]) {
        case '+': return lab_lexer_token_make((int)lab_tok_operator_plus,         NULL, iter);
        case '-': return lab_lexer_token_make((int)lab_tok_operator_minus,        NULL, iter);
        case '*': return lab_lexer_token_make((int)lab_tok_operator_mul,          NULL, iter);
        case '=': return lab_lexer_token_make((int)lab_tok_operator_equals,       NULL, iter);
        case '^': return lab_lexer_token_make((int)lab_tok_operator_xor,          NULL, iter);
        case '|': return lab_lexer_token_make((int)lab_tok_operator_or,           NULL, iter);
        case '!': return lab_lexer_token_make((int)lab_tok_operator_not,          NULL, iter);
        case '/': { // Check to see if comment or not
            if(code[iter->iter + 1]=='/') {
                lab_lexer_iterator_t begin_pos = *iter;
                for(;iter->iter < max_len; lab_lexer_iter_next(code, iter) ) {
                    if(code[iter->iter] == '\n' || code[iter->iter] == '\0') {
                        return lab_lexer_token_make((int)lab_tok_comment,    NULL, &begin_pos);
                    }
                }
                lab_errorln("Failed to find end of comment starting at line: %d, column: %d", begin_pos.line, begin_pos.column);
                return lab_lexer_token_make((int)lab_tok_nil,                NULL, &begin_pos);
            } else {
                return lab_lexer_token_make((int)lab_tok_operator_div,       NULL, iter);
            }
            break;
        }
        case '<': {
            if(code[iter->iter + 1]=='<') {
                lab_lexer_iterator_t begin_pos = *iter;
                lab_lexer_iter_next(code, iter);
                return lab_lexer_token_make((int)lab_tok_operator_bitshiftl, NULL, &begin_pos);
            } else {
                return lab_lexer_token_make((int)lab_tok_operator_lesst,     NULL, iter);
            }
            break;
        }
        case '>': {
            if(code[iter->iter + 1]=='>') {
                lab_lexer_iterator_t begin_pos = *iter;
                lab_lexer_iter_next(code, iter);
                return lab_lexer_token_make((int)lab_tok_operator_bitshiftr, NULL, &begin_pos);
            } else {
                return lab_lexer_token_make((int)lab_tok_operator_greatert,  NULL, iter);
            }
            break;
        }
        default:  return lab_lexer_token_make((int)lab_tok_nil,              NULL, iter);
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
        return lab_lexer_token_make((int)lab_tok_nil, NULL, &begin_pos);
    } else {
        char* buffer = (char*)malloc((end_index - begin_index) + 1);
        if(buffer==NULL) {
            lab_errorln("Failed to allocate string buffer for string starting at line: %d, column: %d", begin_pos.line, begin_pos.column);
            return lab_lexer_token_make((int)lab_tok_nil, NULL, &begin_pos);
        }
        buffer[end_index - begin_index] = '\0';
        memcpy(buffer, code + begin_index + 1, end_index - begin_index);
        return lab_lexer_token_make(mode == 1 ? (int)lab_tok_string : (int)lab_tok_char, buffer, &begin_pos);
    }
    return lab_lexer_token_make((int)lab_tok_nil, NULL, &begin_pos);
}

lab_lexer_token_t eof_callback(const char* code, lab_lexer_iterator_t* iter, size_t max_len, void* user_data) {
    return lab_lexer_token_make((int)lab_tok_eof, NULL, iter);
}

int lab_custom_lexer_lex(lab_lexer_token_container_t* tokens, const char* code, size_t code_len, void* user_data) {
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
        } else if(cur_char=='\0') {
            lab_token_container_append(tokens, eof_callback(code, &pos, code_len, user_data), &pos, code_len);
        } else {
            lab_errorln("Unexpected character \'%c\' at line: %d, column: %d", cur_char, pos.line, pos.column);
        }

    }
    return 0;
}