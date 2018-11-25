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

char* tok_to_string(lab_tokens_e_t tok) {
    char* buffer = NULL;
    switch(tok) {
        TOK_TO_STRING_TEMPLATE(LAB_TOK_ERR, "error")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_IDENTIFIER, "identifier")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_INTEGER, "integer")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_FLOAT, "float")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_CHAR, "char")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_STRING, "string")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_LPAREN, "(")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_RPAREN, ")")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_LBRACKET, "[")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_RBRACKET, "]")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_LCURLY, "{")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_RCURLY, "}")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_COMMA, ",")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_DECIMAL, ".")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_COLON, ":")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_SEMICOLON, ";")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_DOUBLE_COLON, "::")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_RARROW, "->")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_BOOL, "bool")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_INT, "int")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_FLOAT, "float")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_STR, "str")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_STRUCT, "struct")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_SELF, "self")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_RETURN, "return")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_IF, "if")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_ELSE, "else")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_NIL, "nil")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_FOR, "for")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_WHILE, "while")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_BREAK, "break")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_CONTINUE, "continue")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_TRUE, "true")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_KW_FALSE, "false")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_ADD, "+")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_SUB, "-")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_MUL, "*")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_DIV, "/")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_EQUALS, "=")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_LESST, "<")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_GREATERT, ">")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_COMPARE, "==")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_NOT, "!")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_BITSHIFTL, "<<")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_BITSHIFTR, ">>")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_BIT_XOR, "^")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_BIT_AND, "&")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_BIT_OR, "|")
        TOK_TO_STRING_TEMPLATE(LAB_TOK_OPERATOR_CONCAT, "++");
        TOK_TO_STRING_TEMPLATE(LAB_TOK_EOF, "end of file")
        default: {
            break;
        }
    }
    return buffer;
}

static inline bool match_str_rest(const char* str, size_t expected_len, size_t start, size_t length, const char* rest) {

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

        if(!alpha_callback_rule(raw_code[iter->iter + 1]) && !isdigit(raw_code[iter->iter + 1])) {

            const char* ident_start = raw_code + begin_pos.iter;

            switch (ident_start[0]) {

                case 's': {
                    switch(ident_start[1]) {
                        case 't': {
                            switch(ident_start[2]) {
                                case 'r': {
                                    if(iter->iter - begin_pos.iter + 1 == 3) {
                                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_STR, NULL, begin_pos.line, begin_pos.column);
                                        return true;
                                    }
                                    else if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 3, 3, "uct")) {
                                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_STRUCT, NULL, begin_pos.line, begin_pos.column);
                                        return true;
                                    }
                                }
                                break;
                            }
                        }
                        break;

                        case 'e': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 2, "lf")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_SELF, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;
                    }
                }
                break;

                case 'r': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 5, "eturn")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_RETURN, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'i': {
                    switch(ident_start[1]) {
                        case 'f': {
                            if(iter->iter - begin_pos.iter + 1 == 2) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_IF, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;

                        case 'n': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 1, "t")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_INT, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;
                    }
                }
                break;

                case 'e': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 3, "lse")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_ELSE, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'n': {
                    switch(ident_start[1]) {
                        case 'o': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 1, "t")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_NOT, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;

                        case 'i': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 1, "l")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_NIL, NULL, begin_pos.line, begin_pos.column);
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
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_FOR, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;

                        case 'a': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 3, "lse")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_FALSE, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;

                        case 'l': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 3, "oat")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_FLOAT, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;
                    }
                }
                break;

                case 'w': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 4, "hile")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_WHILE, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'b': {
                    switch(ident_start[1]) {
                        case 'r': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 3, "eak")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_BREAK, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;

                        case 'o': {
                            if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 2, 2, "ol")) {
                                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_BOOL, NULL, begin_pos.line, begin_pos.column);
                                return true;
                            }
                        }
                        break;
                    }
                }
                break;

                case 'c': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 7, "ontinue")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_CONTINUE, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 't': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 3, "rue")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_TRUE, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'o': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 1, "r")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_OR, NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                }
                break;

                case 'a': {
                    if(match_str_rest(ident_start, iter->iter - begin_pos.iter + 1, 1, 2, "nd")) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_KW_AND, NULL, begin_pos.line, begin_pos.column);
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

            lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_IDENTIFIER, ident, begin_pos.line, begin_pos.column);
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

    bool is_float = false;

    for(; iter->iter  < code->used_size; lab_lexer_iter_next(code, iter)) {
        if(raw_code[iter->iter + 1]=='.') {
            
            if(is_float) {
                lab_errorln("Multiple decimals in number starting at line %d, column %d", begin_pos.line, begin_pos.column);
                lab_noticeln("Resyncronizing...");
                for(; iter->iter  < code->used_size; lab_lexer_iter_next(code, iter)) {
                    if(raw_code[iter->iter + 1]=='.') {
                        continue;
                    }
                    if(!numeric_callback_rule(raw_code[iter->iter + 1])) {
                        break;
                    }
                }
                lab_noticeln("Continuing at line %d, column %d", iter->line, iter->column);
                lab_errorln("Error here ==> %.*s", (iter->iter - begin_pos.iter) + 1, raw_code + begin_pos.iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_ERR, "MULTIPLE DECIMALS IN NUMBER", begin_pos.line, begin_pos.column);
                return false;
            }
            is_float = true;
            continue;
        }
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
                lab_lexer_token_container_append(tokens, code, iter->iter, is_float ? (int)LAB_TOK_FLOAT : (int)LAB_TOK_INTEGER, num, begin_pos.line, begin_pos.column);
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
        case '(': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_LPAREN,    NULL, iter->line, iter->column); break;
        case ')': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_RPAREN,    NULL, iter->line, iter->column); break;
        case '[': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_LBRACKET,  NULL, iter->line, iter->column); break;
        case ']': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_RBRACKET,  NULL, iter->line, iter->column); break;
        case '{': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_LCURLY,    NULL, iter->line, iter->column); break;
        case '}': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_RCURLY,    NULL, iter->line, iter->column); break;
        case '.': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_DECIMAL,   NULL, iter->line, iter->column); break;
        case ',': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_COMMA,     NULL, iter->line, iter->column); break;
        case ':': {
            if(((char*)code->raw_data)[iter->iter + 1]==':') {
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_DOUBLE_COLON,  NULL, iter->line, iter->column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_COLON,  NULL, iter->line, iter->column);
                return true;
            }

            break;
        }
        case ';': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_SEMICOLON, NULL, iter->line, iter->column); break;
        default:  return false;
    }

    return true;

}

bool operator_callback_rule(char c) { return (c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='^' ||
                                                     c=='&' || c=='<' || c=='>' || c=='|' || c=='!'); }
bool operator_callback(const lab_vec_t* code,
                              lab_lexer_iterator_t* iter, 
                              lab_lexer_token_container_t* tokens, 
                              void* user_data) {

    switch(((char*)code->raw_data)[iter->iter]) {
        case '+': {
            if(((char*)code->raw_data)[iter->iter + 1]=='+') {
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_CONCAT,  NULL, iter->line, iter->column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_ADD,  NULL, iter->line, iter->column);
                return true;
            }

            break;
        }
        case '-': {
            if(((char*)code->raw_data)[iter->iter + 1]=='>') {
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_RARROW,  NULL, iter->line, iter->column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_SUB,  NULL, iter->line, iter->column);
                return true;
            }

            break;
        }
        case '*': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_MUL,    NULL, iter->line, iter->column); break;

        case '=': {
            if(((char*)code->raw_data)[iter->iter + 1]=='=') {
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_COMPARE,  NULL, iter->line, iter->column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_EQUALS,  NULL, iter->line, iter->column);
                return true;
            }

            break;
        }
        case '^': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_BIT_XOR,NULL, iter->line, iter->column); break;
        case '|': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_BIT_OR, NULL, iter->line, iter->column); break;
        case '!': lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_NOT,    NULL, iter->line, iter->column); break;
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
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_DIV, NULL, iter->line, iter->column);
            }
            break;
        }
        case '<': {
            if(((char*)code->raw_data)[iter->iter + 1]=='<') {
                lab_lexer_iterator_t begin_pos = *iter;
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_BITSHIFTL, NULL, begin_pos.line, begin_pos.column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_LESST, NULL, iter->line, iter->column);
                return true;
            }
            break;
        }
        case '>': {
            if(((char*)code->raw_data)[iter->iter + 1]=='>') {
                lab_lexer_iterator_t begin_pos = *iter;
                lab_lexer_iter_next(code, iter);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_BITSHIFTR, NULL, begin_pos.line, begin_pos.column);
                return true;
            } else {
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_OPERATOR_GREATERT, NULL, iter->line, iter->column);
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

        if(raw_code[iter->iter-1]!='\\') {
            if(raw_code[iter->iter]=='\"' && mode == 1) {
                end_of_string = iter->iter - 1;
                break;
            } else if(raw_code[iter->iter]=='\'' && mode == -1) {
                end_of_string = iter->iter -1;
                break;
            }
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
            lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_STRING, buffer, begin_pos.line, begin_pos.column);
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

    lab_lexer_token_container_append(tokens, code, iter->iter, (int)LAB_TOK_EOF, NULL, iter->line, iter->column);
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

    bool was_error = false;

    for (pos.iter = 0; pos.iter < code->used_size; lab_lexer_iter_next(code, &pos)) {
        
        char cur_char = raw_code[pos.iter];
        if(isspace(cur_char)) {
            continue;
        } else if(alpha_callback_rule(cur_char)) {
            if(!alpha_callback(code, &pos, tokens, user_data)) {
                was_error = true;
            }
        } else if (numeric_callback_rule(cur_char)) {
           if(!numeric_callback(code, &pos, tokens, user_data)) {
                was_error = true;
            }
        } else if (symbol_callback_rule(cur_char)) {
            if(!symbol_callback(code, &pos, tokens, user_data)) {
                was_error = true;
            }
        } else if (operator_callback_rule(cur_char)) {
            if(!operator_callback(code, &pos, tokens, user_data)) {
                was_error = true;
            }
        } else if(string_callback_rule(cur_char)) {
            if(!string_callback(code, &pos, tokens, user_data)) {
                was_error = true;
            }
        } else if(eof_callback_rule(cur_char)) {
            if(!eof_callback(code, &pos, tokens, user_data)) {
                was_error = true;
            }
            break;
        } else {
            lab_errorln("Unexpected character \'%c\' at line: %d, column: %d", cur_char, pos.line, pos.column);
        }
    }

    if(was_error) {
        return false;
    } else {
        return true;
    }
}