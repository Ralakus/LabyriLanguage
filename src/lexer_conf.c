#include "lexer_conf.h"
#include <lab/mempool.h>

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
        TOK_TO_STRING_TEMPLATE(lab_tok_kw_as, "as")
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

extern bool alpha_callback_rule(char c) { return (isalpha(c) > 0 || c == '_'); }
extern bool alpha_callback(const lab_vec_t* code,
                           lab_lexer_iterator_t* iter, 
                           lab_lexer_token_container_t* tokens, 
                           void* user_data) {
    
    lab_lexer_iterator_t begin_pos = *iter;
    char*                raw_code  = (char*)code->raw_data;

    static const char reserved[]         = {           "Func\0"         "let\0"         "return\0"           "as" };
    static lab_tokens_e reserved_types[] = { lab_tok_kw_func, lab_tok_kw_let, lab_tok_kw_return,   lab_tok_kw_as    };

    for(; iter->iter < code->used_size; lab_lexer_iter_next(code, iter)) {
        if((!alpha_callback_rule(raw_code[iter->iter + 1])) && (!numeric_callback_rule(raw_code[iter->iter + 1]))) {

            size_t reserved_sub_str = 0;
            bool   matches          = true;
            size_t j                = 0;

            for(size_t i = 0; i < sizeof(reserved); i++, j++) {
                if(reserved[i]=='\0') {
                    j = -1;
                    ++reserved_sub_str;
                    matches = true;
                    continue;
                }

                if(matches) {
                    size_t len = (iter->iter - begin_pos.iter) - 1;
                    if(j > len && reserved[i] == (raw_code + begin_pos.iter)[j]) {
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)reserved_types[reserved_sub_str], NULL, begin_pos.line, begin_pos.column);
                        return true;
                    }
                    if(reserved[i] == (raw_code + begin_pos.iter)[j]) {
                        continue;
                    } else {
                        matches = false;
                    }
                }
            }

            lab_mempool_t* pool = (lab_mempool_t*)user_data;


            lab_mempool_suballoc_t* alloc = lab_mempool_suballoc_alloc(pool, (iter->iter - begin_pos.iter) + 2);
            char* ident = alloc->data;

            if(ident==NULL) {

                lab_errorln("Failed to allocate buffer for identifier token for identifier at line: %d, column: %d!", begin_pos.line, begin_pos.column);
                return false;

            } else {

                ident[(iter->iter - begin_pos.iter) + 1] = '\0';
                memcpy(ident, raw_code + begin_pos.iter, (iter->iter - begin_pos.iter) + 1);
                lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_identifier, ident, begin_pos.line, begin_pos.column);
                return true;

            }

        }
    }

    return true;

}

extern bool whitespace_callback_rule(char c) { return (isspace(c) > 0); }
extern bool whitespace_callback(const lab_vec_t* code,
                                lab_lexer_iterator_t* iter, 
                                lab_lexer_token_container_t* tokens, 
                                void* user_data) {

    return true;

}

extern bool numeric_callback_rule(char c) { return (isdigit(c) > 0 || c == '.'); }
extern bool numeric_callback(const lab_vec_t* code,
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

extern bool symbol_callback_rule(char c) { return (c=='(' || c==')' || c=='[' || c==']' || c=='{' || c=='}' ||
                                                   c==',' || c==':' || c==';'); }
extern bool symbol_callback(const lab_vec_t* code,
                            lab_lexer_iterator_t* iter, 
                            lab_lexer_token_container_t* tokens, 
                            void* user_data){

    switch(((char*)code->raw_data)[iter->iter]) {
        case '(': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_lparen,    NULL, iter->line, iter->column); break;
        case ')': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_rparen,    NULL, iter->line, iter->column); break;
        case '[': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_lbracket,  NULL, iter->line, iter->column); break;
        case ']': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_rbracket,  NULL, iter->line, iter->column); break;
        case '{': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_lcurley,   NULL, iter->line, iter->column); break;
        case '}': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_rcurley,   NULL, iter->line, iter->column); break;
        case ',': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_comma,     NULL, iter->line, iter->column); break;
        case ':': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_colon,     NULL, iter->line, iter->column); break;
        case ';': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_semicolon, NULL, iter->line, iter->column); break;
        default:  return false;
    }

    return true;

}

extern bool operator_callback_rule(char c) { return (c=='+' || c=='-' || c=='*' || c=='/' || c=='=' || c=='^' ||
                                                     c=='&' || c=='<' || c=='>' || c=='|'); }
extern bool operator_callback(const lab_vec_t* code,
                              lab_lexer_iterator_t* iter, 
                              lab_lexer_token_container_t* tokens, 
                              void* user_data) {

    switch(((char*)code->raw_data)[iter->iter]) {
        case '+': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_plus,   NULL, iter->line, iter->column); break;
        case '-': lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_operator_minus,  NULL, iter->line, iter->column); break;
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
                        lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_comment, NULL, begin_pos.line, begin_pos.column);
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

extern bool string_callback_rule(char c) { return (c == '\"' || c == '\''); }
extern bool string_callback(const lab_vec_t* code,
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

extern bool eof_callback_rule(char c) { return (c == '\0'); }
extern bool eof_callback(const lab_vec_t* code,
                         lab_lexer_iterator_t* iter, 
                         lab_lexer_token_container_t* tokens, 
                         void* user_data) {

    lab_lexer_token_container_append(tokens, code, iter->iter, (int)lab_tok_eof, NULL, iter->line, iter->column);
    return true;

}

extern bool lab_custom_lexer_lex(lab_lexer_token_container_t* tokens, 
                                 lab_lexer_ruleset_t* ruleset, 
                                 const lab_vec_t* code, 
                                 void* user_data) {


    lab_lexer_iterator_t pos;
    pos.iter   = 0;
    pos.line   = 1;
    pos.column = 1;

    char* raw_code = (char*)code->raw_data;

    for (pos.iter = 0; pos.iter < code->used_size; lab_lexer_iter_next(code, &pos)) {
        
        char cur_char = raw_code[pos.iter];

        if(isalpha(cur_char)) {
            alpha_callback(code, &pos, tokens, user_data);
        } else if (isdigit(cur_char) || cur_char == '.') {
           numeric_callback(code, &pos, tokens, user_data);
        } else if(isspace(cur_char)) {
            continue;
            // lab_token_container_append(tokens, whitespace_callback(code, &pos, code_len, user_data), &pos, code_len);
        } else if (cur_char=='(' || cur_char==')' || cur_char=='[' || cur_char==']' || cur_char=='{' || cur_char=='}' ||
                   cur_char==',' || cur_char==':' || cur_char==';') {
            symbol_callback(code, &pos, tokens, user_data);
        } else if (cur_char=='+' || cur_char=='-' || cur_char=='*' || cur_char=='/' || cur_char=='=' || cur_char=='^' ||
                   cur_char=='&' || cur_char=='<' || cur_char=='>' || cur_char=='|') {
            operator_callback(code, &pos, tokens, user_data);
        } else if(cur_char=='\"' || cur_char=='\'') {
            string_callback(code, &pos, tokens, user_data);
        } else if(cur_char=='\0') {
            eof_callback(code, &pos, tokens, user_data);
            break;
        } else {
            lab_errorln("Unexpected character \'%c\' at line: %d, column: %d", cur_char, pos.line, pos.column);
        }
    }

    return true;
}