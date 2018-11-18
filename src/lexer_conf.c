#include "lexer_conf.h"
#include <lab/mempool.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

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
        TOK_TO_STRING_TEMPLATE(lab_tok_err, "error")
        TOK_TO_STRING_TEMPLATE(lab_tok_identifier, "identifier")
        TOK_TO_STRING_TEMPLATE(lab_tok_number, "number")
        TOK_TO_STRING_TEMPLATE(lab_tok_char, "char")
        TOK_TO_STRING_TEMPLATE(lab_tok_string, "string")
        TOK_TO_STRING_TEMPLATE(lab_tok_lparen, "(")
        TOK_TO_STRING_TEMPLATE(lab_tok_rparen, ")")
        TOK_TO_STRING_TEMPLATE(lab_tok_lbracket, "[")
        TOK_TO_STRING_TEMPLATE(lab_tok_rbracket, "]")
        TOK_TO_STRING_TEMPLATE(lab_tok_lcurly, "{")
        TOK_TO_STRING_TEMPLATE(lab_tok_rcurly, "}")
        TOK_TO_STRING_TEMPLATE(lab_tok_comma, ",")
        TOK_TO_STRING_TEMPLATE(lab_tok_decimal, ".")
        TOK_TO_STRING_TEMPLATE(lab_tok_colon, ":")
        TOK_TO_STRING_TEMPLATE(lab_tok_semicolon, ";")
        TOK_TO_STRING_TEMPLATE(lab_tok_double_colon, "::")
        TOK_TO_STRING_TEMPLATE(lab_tok_rarrow, "->")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_var, "var")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_struct, "struct")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_self, "self")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_return, "return")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_if, "if")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_else, "else")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_nil, "nil")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_for, "for")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_while, "while")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_break, "break")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_continue, "continue")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_true, "true")
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_false, "false")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_add, "+")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_sub, "-")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_mul, "*")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_div, "/")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_equals, "=")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_xor, "^")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_and, "&")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_lesst, "<")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_greatert, ">")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_or, "|")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_not, "!")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_bitshiftl, "<<")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_bitshiftr, ">>")
        TOK_TO_STRING_TEMPLATE(lab_tok_operator_concat, "++");
        TOK_TO_STRING_TEMPLATE(lab_tok_eof, "end of file")
        default: {
            break;
        }
    }
    return buffer;
}

bool match_str_rest(const char* str, size_t expected_len, size_t start, size_t length, const char* rest) {

    if(expected_len == (start + length)) {
        if(memcmp(str + start, rest, length) == 0) {
            return true;
        }
    }

    return false;
}

bool alpha_callback_rule(char c) { return (isalpha(c) > 0 || c == '_'); }
bool alpha_callback(const lab_vec_t* code,
                           lab_lexer_iterator_t* iter, 
                           lab_lexer_token_container_t* tokens, 
                           void* user_data) {

    lab_lexer_iterator_t begin_pos = *iter;
    char*                raw_code  = (char*)code->raw_data;

    for(;iter->iter < code->used_size; lab_lexer_iter_next(code, iter) ) {

        if(!alpha_callback_rule(raw_code[iter->iter + 1])) {

            const char* ident_start = raw_code + begin_pos.iter;

            switch (ident_start[0]) {

                case 'v': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 2, "ar")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_var, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 's': {
                    switch(ident_start[1]) {
                        case 't': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 4, "ruct")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_struct, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;

                        case 'e': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 2, "lf")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_self, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;
                    }
                }
                break;

                case 'r': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 5, "eturn")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_return, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'i': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 1, "f")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_if, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'e': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 3, "lse")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_else, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'n': {
                    switch(ident_start[1]) {
                        case 'o': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 1, "t")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_not, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;

                        case 'i': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 1, "l")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_nil, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;
                    }
                }
                break;

                case 'f': {
                    switch(ident_start[1]) {
                        case 'o': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 1, "r")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_for, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;

                        case 'a': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 3, "lse")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_false, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;
                    }
                }
                break;

                case 'w': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 4, "hile")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_while, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'b': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 4, "reak")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_break, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'c': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 7, "ontinue")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_continue, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 't': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 3, "rue")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_kw_if, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                default:
                    break;
            }

            lab_mempool_t* pool = (lab_mempool_t*)user_data;

            lab_mempool_suballoc_t* alloc = lab_mempool_suballoc_alloc(pool, (iter->iter - begin_pos.iter) + 2);
            char* ident = alloc->data;

            if(ident==NULL) {

                lab_errorln("Failed to allocate buffer for identifier token for identifier at line: %d, column: %d!", begin_pos.line, begin_pos.column);

            } else {

                ident[(iter->iter - begin_pos.iter) + 1] = '\0';
                memcpy(ident, raw_code + begin_pos.iter, (iter->iter - begin_pos.iter) + 1);

            }

            lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_identifier, ident, begin_pos.line, begin_pos.column);
            return true;
        }
    }

    return true;

}

bool whitespace_callback_rule(char c) { return (isspace(c) > 0); }
bool whitespace_callback(const lab_vec_t* code,
                                lab_lexer_iterator_t* iter, 
                                lab_lexer_token_container_t* tokens, 
                                void* user_data) {

    return true;

}

bool numeric_callback_rule(char c) { return (isdigit(c) > 0); }
bool numeric_callback(const lab_vec_t* code,
                             lab_lexer_iterator_t* iter, 
                             lab_lexer_token_container_t* tokens, 
                             void* user_data) {

    lab_lexer_iterator_t begin_pos = *iter;
    char*                raw_code  = (char*)code->raw_data;

    for(; iter->iter  < code->used_size; lab_lexer_iter_next(code, iter)) {
        if(!numeric_callback_rule(raw_code[iter->iter + 1])) {
            lab_mempool_t* pool = (lab_mempool_t*)user_data;
            lab_mempool_suballoc_t* alloc = lab_mempool_suballoc_alloc(pool, (iter->iter - begin_pos.iter) + 2);

            char* num = alloc->data;
            if(num==NULL) {

                lab_errorln("Failed to allocate buffer for numerical token for number at line: %d, column: %d!", begin_pos.line, begin_pos.column);
                return false;

            } else {

                num[(iter->iter - begin_pos.iter) + 1] = '\0';
                memcpy(num, raw_code + begin_pos.iter, (iter->iter - begin_pos.iter) + 1);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_number, num, begin_pos.line, begin_pos.column);
                return true;
            }
        }
    }

    return true;

}

bool symbol_callback_rule(char c) { return (c=='(' || c==')' || c=='[' || c==']' || c=='{' || c=='}' || c=='.' ||
                                                   c==',' || c==':' || c==';'); }
bool symbol_callback(const lab_vec_t* code,
                            lab_lexer_iterator_t* iter, 
                            lab_lexer_token_container_t* tokens, 
                            void* user_data){

    switch(((char*)code->raw_data)[iter->iter]) {
        case '(': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_lparen,    NULL, iter->line, iter->column); break;
        case ')': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_rparen,    NULL, iter->line, iter->column); break;
        case '[': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_lbracket,  NULL, iter->line, iter->column); break;
        case ']': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_rbracket,  NULL, iter->line, iter->column); break;
        case '{': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_lcurly,   NULL, iter->line, iter->column); break;
        case '}': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_rcurly,   NULL, iter->line, iter->column); break;
        case '.': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_decimal,   NULL, iter->line, iter->column); break;
        case ',': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_comma,     NULL, iter->line, iter->column); break;
        case ':': {
            if(((char*)code->raw_data)[iter->iter + 1]==':') {
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_double_colon,  NULL, iter->line, iter->column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_colon,  NULL, iter->line, iter->column);
                return true;
            }

            break;
        }
        case ';': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_semicolon, NULL, iter->line, iter->column); break;
        default:  return false;
    }

    return true;

}

bool operator_callback_rule(char c) { return (c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='^' ||
                                                     c=='&' || c=='<' || c=='>' || c=='|'); }
bool operator_callback(const lab_vec_t* code,
                              lab_lexer_iterator_t* iter, 
                              lab_lexer_token_container_t* tokens, 
                              void* user_data) {

    switch(((char*)code->raw_data)[iter->iter]) {
        case '+': {
            if(((char*)code->raw_data)[iter->iter + 1]=='+') {
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_concat,  NULL, iter->line, iter->column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_add,  NULL, iter->line, iter->column);
                return true;
            }

            break;
        }
        case '-': {
            if(((char*)code->raw_data)[iter->iter + 1]=='>') {
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_rarrow,  NULL, iter->line, iter->column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_sub,  NULL, iter->line, iter->column);
                return true;
            }

            break;
        }
        case '*': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_mul,    NULL, iter->line, iter->column); break;
        case '=': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_equals, NULL, iter->line, iter->column); break;
        case '^': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_xor,    NULL, iter->line, iter->column); break;
        case '|': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_or,     NULL, iter->line, iter->column); break;
        case '!': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_not,    NULL, iter->line, iter->column); break;
        case '/': { // Check to see if comment or not
            if(((char*)code->raw_data)[iter->iter + 1]=='/') {
                lab_lexer_iterator_t begin_pos = *iter;
                for(;iter->iter < code->used_size; lab_lexer_iter_next(code, iter) ) {
                    if(((char*)code->raw_data)[iter->iter] == '\n' || ((char*)code->raw_data)[iter->iter] == '\0') {
                        return true;
                    }
                }
                lab_errorln("Failed to find end of comment starting at line: %d, column: %d", begin_pos.line, begin_pos.column);
                return false;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_div, NULL, iter->line, iter->column);
            }
            break;
        }
        case '<': {
            if(((char*)code->raw_data)[iter->iter + 1]=='<') {
                lab_lexer_iterator_t begin_pos = *iter;
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_bitshiftl, NULL, begin_pos.line, begin_pos.column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_lesst, NULL, iter->line, iter->column);
                return true;
            }
            break;
        }
        case '>': {
            if(((char*)code->raw_data)[iter->iter + 1]=='>') {
                lab_lexer_iterator_t begin_pos = *iter;
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_bitshiftr, NULL, begin_pos.line, begin_pos.column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_greatert, NULL, iter->line, iter->column);
                return true;
            }
            break;
        }
        default:  break;
    }

    return true;

}

bool string_callback_rule(char c) { return (c == '\"' || c == '\''); }
bool string_callback(const lab_vec_t* code,
                            lab_lexer_iterator_t* iter, 
                            lab_lexer_token_container_t* tokens, 
                            void* user_data) {

    lab_lexer_iterator_t begin_pos = *iter;
    char*                raw_code  = (char*)code->raw_data;

    int mode = raw_code[iter->iter]=='\"' ? 1 : -1; // 1 means it's lexing a string, -1 means char
    size_t end_of_string = 0;

    lab_lexer_iter_next(code, iter); // To skip the first " or '

    for(; iter->iter < code->used_size; lab_lexer_iter_next(code, iter)) {

        if(raw_code[iter->iter]=='\"' && mode == 1) {
            end_of_string = iter->iter - 1;
            break;
        } else if(raw_code[iter->iter]=='\'' && mode == -1) {
            end_of_string = iter->iter -1;
            break;
        }

    }

    if(end_of_string==0) {
        lab_errorln("Failed to find end of string declared at line: %d, column: %d!", begin_pos.line, begin_pos.column);
        return false;
    } else {

        lab_mempool_t* pool = (lab_mempool_t*)user_data;

        lab_mempool_suballoc_t* alloc = lab_mempool_suballoc_alloc(pool, (end_of_string - begin_pos.iter) + 1);

        char* buffer = alloc->data;

        if(buffer==NULL) {
            lab_errorln("Failed to allocate string buffer for string declared at line: %d, column: %d", begin_pos.line, begin_pos.column);
            return false;
        } else {

            buffer[end_of_string - begin_pos.iter] = '\0';
            memcpy(buffer, raw_code + begin_pos.iter + 1, end_of_string - begin_pos.iter);
            lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_string, buffer, begin_pos.line, begin_pos.column);
            return true;

        }

    }

    return true;

}

bool eof_callback_rule(char c) { return (c == '\0'); }
bool eof_callback(const lab_vec_t* code,
                         lab_lexer_iterator_t* iter, 
                         lab_lexer_token_container_t* tokens, 
                         void* user_data) {

    lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_eof, NULL, iter->line, iter->column);
    return true;

}

bool lab_custom_lexer_lex(lab_lexer_token_container_t* tokens, 
                                 const lab_vec_t* code, 
                                 void* user_data) {


    lab_lexer_iterator_t pos;
    pos.iter   = 0;
    pos.line   = 1;
    pos.column = 1;

    char* raw_code = (char*)code->raw_data;

    for (pos.iter = 0; pos.iter < code->used_size; lab_lexer_iter_next(code, &pos)) {
        
        char cur_char = raw_code[pos.iter];

        if(alpha_callback_rule(cur_char)) {
            alpha_callback(code, &pos, tokens, user_data);
        } else if (numeric_callback_rule(cur_char)) {
           numeric_callback(code, &pos, tokens, user_data);
        } else if(isspace(cur_char)) {
            continue;
            // lab_token_container_append(tokens, whitespace_callback(code, &pos, code_len, user_data), &pos, code_len);
        } else if (symbol_callback_rule(cur_char)) {
            symbol_callback(code, &pos, tokens, user_data);
        } else if (operator_callback_rule(cur_char)) {
            operator_callback(code, &pos, tokens, user_data);
        } else if(string_callback_rule(cur_char)) {
            string_callback(code, &pos, tokens, user_data);
        } else if(eof_callback_rule(cur_char)) {
            eof_callback(code, &pos, tokens, user_data);
            break;
        } else {
            lab_errorln("Unexpected character \'%c\' at line: %d, column: %d", cur_char, pos.line, pos.column);
        }
    }

    return true;
}