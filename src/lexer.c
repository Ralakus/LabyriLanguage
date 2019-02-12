#include <string.h>
#include "lexer.h"

 #define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

bool lab_lexer_token_container_init(lab_lexer_token_container_t* container, size_t init_size) {
    if(!lab_vec_init(&container->tokens, sizeof(lab_lexer_token_t), init_size)) {
        return false;
    } else {
        container->code = NULL;
        return true;
    }
}

void lab_lexer_token_container_free(lab_lexer_token_container_t* container) {
    lab_vec_free(&container->tokens);
    container->code = NULL;
}

bool lab_lexer_token_container_append(lab_lexer_token_container_t* container,
                                      size_t code_size,
                                      lab_lexer_iter_t iter,
                                      lab_tokens_e_t type,
                                      const char* data,
                                      size_t data_len) {

    ++container->tokens.used_size;
    if(container->tokens.used_size > container->tokens.alloc_size) {
        if(!lab_vec_resize(&container->tokens, (
            container->tokens.used_size + (code_size / max(iter.i + (code_size % max(iter.i, 1) != 0), 1))
        ))) {
            lab_errorln("Failed to resize token container after %d tokens!", container->tokens.used_size);
            return false;
        }
    }

    lab_lexer_token_t* tok = (lab_lexer_token_t*)lab_vec_at(&container->tokens, container->tokens.used_size - 1);
    tok->type     = type;
    tok->data     = data;
    tok->data_len = data_len;
    tok->line     = iter.line;
    tok->column   = iter.column;

    return true;

}

static inline bool match_str_rest(const char* str, size_t expected_len, size_t start, size_t length, const char* rest) {

    if(expected_len == (start + length)) {
        if(memcmp(str + start, rest, length) == 0) {
            return true;
        }
    }

    return false;
}


bool lab_lexer_lex(lab_lexer_token_container_t* container, const char* code) {

#define CREATE_TOK(tok_iter, tok, data, data_len) lab_lexer_token_container_append(container, code_len, tok_iter, tok, data, data_len)
#define NEXT() lab_lexer_iter_next(code, &iter)

    size_t code_len = strlen(code);
    container->code = code;

    bool was_error = false;

    for(lab_lexer_iter_t iter = {0,1,1}; iter.i < code_len + 1;) {

        char cur_char = code[iter.i];

        switch(cur_char) {

            // Skip whitespace
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case '\v':
            case '\f':
                NEXT();
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
                lab_lexer_iter_t start = iter;
                while(NEXT(), code[iter.i] >= '0' && code[iter.i] <= '9');
                if(code[iter.i] == '.') {
                    while(NEXT(), code[iter.i] >= '0' && code[iter.i] <= '9');
                    CREATE_TOK(start, LAB_TOK_FLOAT, &code[start.i], iter.i - start.i);
                } else {
                    CREATE_TOK(start, LAB_TOK_FLOAT, &code[start.i], iter.i - start.i);
                }
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
                lab_lexer_iter_t start = iter;
                while(NEXT(), (code[iter.i] >= 'a' && code[iter.i] <= 'z') ||
                              (code[iter.i] >= 'A' && code[iter.i] <= 'Z') ||
                              (code[iter.i] >= '0' && code[iter.i] <= '9') ||
                               code[iter.i] == '_');
                bool matched = false;
                switch (code[start.i]) {

                    case 's': {
                        switch(code[start.i + 1]) {
                            case 't': {
                                switch(code[start.i + 2]) {
                                    case 'r': {
                                        if(iter.i - start.i == 3) {
                                            CREATE_TOK(start, LAB_TOK_KW_STR, &code[start.i], 0);
                                            matched = true;
                                            break;
                                        }
                                        else if(match_str_rest(&code[start.i], iter.i - start.i, 3, 3, "uct")) {
                                            CREATE_TOK(start, LAB_TOK_KW_STRUCT, &code[start.i], 0);
                                            matched = true;
                                            break;
                                        }
                                    }
                                    break;
                                }
                            }
                            break;

                            case 'e': {
                                if(match_str_rest(&code[start.i], iter.i - start.i, 2, 2, "lf")) {
                                    CREATE_TOK(start, LAB_TOK_KW_SELF, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;

                    case 'r': {
                        if(match_str_rest(&code[start.i], iter.i - start.i, 1, 5, "eturn")) {
                            CREATE_TOK(start, LAB_TOK_KW_RETURN, &code[start.i], 0);
                            matched = true;
                            break;
                        }
                    }
                    break;

                    case 'i': {
                        switch(code[start.i + 1]) {
                            case 'f': {
                                if(iter.i - start.i == 2) {
                                    CREATE_TOK(start, LAB_TOK_KW_IF, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;

                            case 'n': {
                                if(match_str_rest(&code[start.i], iter.i - start.i, 2, 1, "t")) {
                                    CREATE_TOK(start, LAB_TOK_KW_INT, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;

                    case 'e': {
                        if(match_str_rest(&code[start.i], iter.i - start.i, 1, 3, "lse")) {
                            CREATE_TOK(start, LAB_TOK_KW_ELSE, &code[start.i], 0);
                            matched = true;
                            break;
                        }
                    }
                    break;

                    case 'n': {
                        switch(code[start.i + 1]) {
                            case 'o': {
                                if(match_str_rest(&code[start.i], iter.i - start.i, 2, 1, "t")) {
                                    CREATE_TOK(start, LAB_TOK_KW_NOT, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;

                            case 'i': {
                                if(match_str_rest(&code[start.i], iter.i - start.i, 2, 1, "l")) {
                                    CREATE_TOK(start, LAB_TOK_KW_NIL, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;

                    case 'f': {
                        switch(code[start.i + 1]) {
                            case 'o': {
                                if(match_str_rest(&code[start.i], iter.i - start.i, 2, 1, "r")) {
                                    CREATE_TOK(start, LAB_TOK_KW_FOR, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;

                            case 'a': {
                                if(match_str_rest(&code[start.i], iter.i - start.i, 2, 3, "lse")) {
                                    CREATE_TOK(start, LAB_TOK_KW_FALSE, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;

                            case 'l': {
                                if(match_str_rest(&code[start.i], iter.i - start.i, 2, 3, "oat")) {
                                    CREATE_TOK(start, LAB_TOK_KW_FLOAT, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;

                    case 'w': {
                        if(match_str_rest(&code[start.i], iter.i - start.i, 1, 4, "hile")) {
                            CREATE_TOK(start, LAB_TOK_KW_WHILE, &code[start.i], 0);
                            matched = true;
                            break;
                        }
                    }
                    break;

                    case 'b': {
                        switch(code[start.i + 1]) {
                            case 'r': {
                                if(match_str_rest(&code[start.i], iter.i - start.i, 2, 3, "eak")) {
                                    CREATE_TOK(start, LAB_TOK_KW_BREAK, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;

                            case 'o': {
                                if(match_str_rest(&code[start.i], iter.i - start.i, 2, 2, "ol")) {
                                    CREATE_TOK(start, LAB_TOK_KW_BOOL, &code[start.i], 0);
                                    matched = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }
                    break;

                    case 'c': {
                        if(match_str_rest(&code[start.i], iter.i - start.i, 1, 7, "ontinue")) {
                            CREATE_TOK(start, LAB_TOK_KW_CONTINUE, &code[start.i], 0);
                            matched = true;
                            break;
                        }
                    }
                    break;

                    case 't': {
                        if(match_str_rest(&code[start.i], iter.i - start.i, 1, 3, "rue")) {
                            CREATE_TOK(start, LAB_TOK_KW_TRUE, &code[start.i], 0);
                            matched = true;
                            break;
                        }
                    }
                    break;

                    case 'o': {
                        if(match_str_rest(&code[start.i], iter.i - start.i, 1, 1, "r")) {
                            CREATE_TOK(start, LAB_TOK_KW_OR, &code[start.i], 0);
                            matched = true;
                            break;
                        }
                    }
                    break;

                    case 'a': {
                        if(match_str_rest(&code[start.i], iter.i - start.i, 1, 2, "nd")) {
                            CREATE_TOK(start, LAB_TOK_KW_AND, &code[start.i], 0);
                            matched = true;
                            break;
                        }
                    }
                    break;

                    default:
                        break;
                }
                if(!matched){
                    CREATE_TOK(start, LAB_TOK_IDENTIFIER, &code[start.i], iter.i - start.i);
                }
            }
            break;

            // String
            case '\'':
            case '\"': {
                lab_lexer_iter_t start = iter;
                NEXT();
                while(code[iter.i] != '\"') {
                    if(code[iter.i + 1] == '\0') {
                        was_error = true;
                        lab_errorln("Failed to find end of string declared at line: %d, column: %d!", start.line, start.column);
                        CREATE_TOK(start, LAB_TOK_ERR, NULL, 0);
                        break;
                    } else {
                        NEXT();
                    }
                }
                if(!was_error) {
                    CREATE_TOK(start, LAB_TOK_STRING, &code[start.i + 1], iter.i - start.i - 1);
                    NEXT();
                }
            }
            break;

            // Operators
            case '+': CREATE_TOK(iter, LAB_TOK_OPERATOR_ADD,      NULL, 0); NEXT(); break;
            case '-': CREATE_TOK(iter, 
                (code[iter.i + 1] == '>' ? (NEXT(), LAB_TOK_RARROW) : LAB_TOK_OPERATOR_SUB),
                NULL, 0); NEXT(); break;
            case '*': CREATE_TOK(iter, LAB_TOK_OPERATOR_MUL,      NULL, 0); NEXT(); break;
            case '/': CREATE_TOK(iter, LAB_TOK_OPERATOR_DIV,      NULL, 0); NEXT(); break;
            case '=': CREATE_TOK(iter, LAB_TOK_OPERATOR_EQUALS,   NULL, 0); NEXT(); break;
            case '<': CREATE_TOK(iter, LAB_TOK_OPERATOR_LESST,    NULL, 0); NEXT(); break;
            case '>': CREATE_TOK(iter, LAB_TOK_OPERATOR_GREATERT, NULL, 0); NEXT(); break;

            // Symbols
            case '(': CREATE_TOK(iter, LAB_TOK_LPAREN,    NULL, 0); NEXT(); break;
            case ')': CREATE_TOK(iter, LAB_TOK_RPAREN,    NULL, 0); NEXT(); break;
            case '{': CREATE_TOK(iter, LAB_TOK_LCURLY,    NULL, 0); NEXT(); break;
            case '}': CREATE_TOK(iter, LAB_TOK_RCURLY,    NULL, 0); NEXT(); break;
            case '[': CREATE_TOK(iter, LAB_TOK_LBRACKET,  NULL, 0); NEXT(); break;
            case ']': CREATE_TOK(iter, LAB_TOK_RBRACKET,  NULL, 0); NEXT(); break;
            case '.': CREATE_TOK(iter, LAB_TOK_DECIMAL,   NULL, 0); NEXT(); break;
            case ',': CREATE_TOK(iter, LAB_TOK_COMMA,     NULL, 0); NEXT(); break;
            case ':': CREATE_TOK(iter, 
                (code[iter.i + 1] == ':' ? (NEXT(), LAB_TOK_DOUBLE_COLON) : LAB_TOK_COLON),
                NULL, 0); NEXT(); break;
            case ';': CREATE_TOK(iter, LAB_TOK_SEMICOLON, NULL, 0); NEXT(); break;

            // Comment
            case '#': {
                lab_lexer_iter_t start = iter;
                while(NEXT(), (code[iter.i] != '\n' && code[iter.i] != '\0'));
                CREATE_TOK(start, LAB_TOK_COMMENT, &code[start.i + 1], iter.i - start.i - 1);
            }
            break;

            // End of string
            case '\0': CREATE_TOK(iter, LAB_TOK_EOF, NULL, 0); NEXT(); break;

            // Other
            default:
                lab_errorln("Unexpected character \'%c\' at line: %d, column: %d", cur_char, iter.line, iter.column);
                was_error = true;
                NEXT();
                break;

        }

    }

    if(!was_error) {
        return true;
    } else {
        return false;
    }

#undef CREATE_TOK

}

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

const char* lab_token_to_string_lookup[47] = {
    "error",               // LAB_TOK_ERR

    "comment",             // LAB_TOK_COMMENT

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
};

void lab_lexer_token_container_print(lab_lexer_token_container_t* container) {

#define PRINT_LINE "-------------------------------------------------------------------------"

    lab_noticeln(LAB_ANSI_COLOR_CYAN"%.22s%s-|-%.32sline, column"LAB_ANSI_COLOR_RESET, PRINT_LINE, "Token type", "Token data"PRINT_LINE);

    for(size_t i = 0; i < container->tokens.used_size; i++) {
        lab_lexer_token_t* tok = (lab_lexer_token_t*)lab_vec_at(&container->tokens, i);

        if(tok->type == LAB_TOK_ERR) {
            lab_errorln(LAB_ANSI_COLOR_RED"%25.25s""%7.25s"
                        " : "
                        "%-32.*s"LAB_ANSI_COLOR_RESET
                        "("
                        LAB_ANSI_COLOR_RED"%4d"LAB_ANSI_COLOR_RESET
                        ", "
                        LAB_ANSI_COLOR_RED"%4d"LAB_ANSI_COLOR_RESET
                        ")",
                        PRINT_LINE, "<ERROR>", tok->data_len, tok->data, tok->line, tok->column
            );
        } else {

            lab_println(LAB_ANSI_COLOR_GREEN"%32.32s"LAB_ANSI_COLOR_RESET
                        " : "
                        LAB_ANSI_COLOR_YELLOW"%-32.*s"LAB_ANSI_COLOR_RESET
                        "("
                        LAB_ANSI_COLOR_RED"%4d"LAB_ANSI_COLOR_RESET
                        ", "
                        LAB_ANSI_COLOR_RED"%4d"LAB_ANSI_COLOR_RESET
                        ")",
                        lab_token_to_string_lookup[tok->type], (tok->data_len == 0 ? 1 : tok->data_len) > 32 ? 32 : tok->data_len, tok->data == NULL ? " " : tok->data, tok->line, tok->column
            );

        }

    }

    lab_noticeln(LAB_ANSI_COLOR_CYAN"%.32s---%.32s------------"LAB_ANSI_COLOR_RESET, PRINT_LINE, PRINT_LINE);


#undef PRINT_LINE

}