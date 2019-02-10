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

typedef struct lab_lexer_iter {
    size_t i;
    size_t line;
    size_t column;
} lab_lexer_iter_t;

void lab_lexer_iter_next(const char* code, lab_lexer_iter_t* iter) {

    ++iter->i;

    if(code[iter->i] == '\n') {

        ++iter->line;
          iter->column = 0;

    } else {

        ++iter->column;
    }

}

void lab_lexer_iter_prev(const char* code, lab_lexer_iter_t* iter) {

    --iter->i;

    if(code[iter->i] == '\n') {

        --iter->line;
          iter->column = 1;
        for(size_t i = iter->i; i > 0; i--) {
            if(code[i] == '\n') {
                break;
            } else {
                ++iter->column;
            }
        }

    } else {

        --iter->column;

    }

}

bool lab_lexer_lex(lab_lexer_token_container_t* container, const char* code) {
    size_t code_len = strlen(code);

    bool was_error = false;

    for(lab_lexer_iter_t iter = {0,1,1}; iter.i < code_len; lab_lexer_iter_next(code, &iter)) {

        char cur_char = code[iter.i];

        switch(cur_char) {

            // Skip whitespace
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '\v':
            case '\f':
                break;
            

            // Numeric
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': {

            }
            break;

            // Alpha
            case '_':
            case 'a':
            case 'b':
            case 'c':
            case 'd':
            case 'e':
            case 'f':
            case 'g':
            case 'h':
            case 'i':
            case 'j':
            case 'k':
            case 'l':
            case 'm':
            case 'n':
            case 'o':
            case 'p':
            case 'q':
            case 'r':
            case 's':
            case 't':
            case 'u':
            case 'v':
            case 'w':
            case 'x':
            case 'y':
            case 'z':
            case 'A':
            case 'B':
            case 'C':
            case 'D':
            case 'E':
            case 'F':
            case 'G':
            case 'H':
            case 'I':
            case 'J':
            case 'K':
            case 'L':
            case 'M':
            case 'N':
            case 'O':
            case 'P':
            case 'Q':
            case 'R':
            case 'S':
            case 'T':
            case 'U':
            case 'V':
            case 'W':
            case 'X':
            case 'Y':
            case 'Z': {

            }
            break;

            // String
            case '\'':
            case '\"': {

            }
            break;

            // Operators
            case '+':
            case '-':
            case '*':
            case '/':
            case '=':
            case '<':
            case '>':
                break;

            // Symbols
            case '(':
            case ')':
            case '{':
            case '}':
            case '[':
            case ']':
            case '.':
            case ',':
            case ':':
            case ';':
                break;

            // End of string
            case '\0':
                break;

            // Other
            default:
                lab_errorln("Unexpected character \'%c\' at line: %d, column: %d", cur_char, iter.line, iter.column);
                was_error = true;
                break;

        }

    }

    if(!was_error) {
        return true;
    } else {
        return false;
    }

}

const char* lab_token_to_string_lookup[45] = {
    "error",               // LAB_TOK_ERR

    "identifier",          // LAB_TOK_IDENTIFIER
    "integer",             // LAB_TOK_INTEGER
    "float",               // LAB_TOK_FLOAT

    "char",                // LAB_TOK_CHAR
    "string",              // LAB_TOK_STRING

    "(",                   // LAB_TOK_LPAREN
    ")",                   // LAB_TOK_RPAREN
    "[",                   // LAB_TOK_LBRACKET
    "]",                   // LAB_TOK_RBRACKET
    "{",                   // LAB_TOK_LCURLY
    "}",                   // LAB_TOK_RCURLY

    ",",                   // LAB_TOK_COMMA
    ".",                   // LAB_TOK_DECIMAL
    ":",                   // LAB_TOK_COLON
    ";",                   // LAB_TOK_SEMICOLON
    "::",                  // LAB_TOK_DOUBLE_COLON,
    "->",                  // LAB_TOK_RARROW

    "bool",                // LAB_TOK_KW_BOOL
    "int",                 // LAB_TOK_KW_INT
    "float",               // LAB_TOK_KW_FLOAT
    "str",                 // LAB_TOK_KW_STR
    "struct",              // LAB_TOK_KW_STRUCT
    "self",                // LAB_TOK_KW_SELF
    "return",              // LAB_TOK_KW_RETURN
    "if",                  // LAB_TOK_KW_IF
    "else",                // LAB_TOK_KW_ELSE
    "nil",                 // LAB_TOK_KW_NIL
    "for",                 // LAB_TOK_KW_FOR
    "while",               // LAB_TOK_KW_WHILE
    "break",               // LAB_TOK_KW_BREAK
    "continue",            // LAB_TOK_KW_CONTINUE
    "true",                // LAB_TOK_KW_TRUE
    "false",               // LAB_TOK_KW_FALSE
    "and",                 // LAB_TOK_KW_AND
    "or",                  // LAB_TOK_KW_OR
    "not",                 // LAB_TOK_KW_NOT,    


    "+",                   // LAB_TOK_OPERATOR_ADD
    "-",                   // LAB_TOK_OPERATOR_SUB
    "*",                   // LAB_TOK_OPERATOR_MUL
    "/",                   // LAB_TOK_OPERATOR_DIV
    "=",                   // LAB_TOK_OPERATOR_EQUALS
    "<",                   // LAB_TOK_OPERATOR_LESST
    ">",                   // LAB_TOK_OPERATOR_GREATERT
    "==",                  // LAB_TOK_OPERATOR_COMPARE
    /*
    "!",                   // LAB_TOK_OPERATOR_NOT
    "<<",                  // LAB_TOK_OPERATOR_BITSHIFTL
    ">>",                  // LAB_TOK_OPERATOR_BITSHIFTR
    "^",                   // LAB_TOK_OPERATOR_BIT_XOR
    "&",                   // LAB_TOK_OPERATOR_BIT_AND
    "|",                   // LAB_TOK_OPERATOR_BIT_OR
    */

    "end of file",         // LAB_TOK_EOF
}