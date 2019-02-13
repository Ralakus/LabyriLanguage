#pragma once

typedef enum lab_tokens_e {
    LAB_TOK_ERR,           //

    LAB_TOK_COMMENT,       // #

    LAB_TOK_IDENTIFIER,    // 
    LAB_TOK_INTEGER,       // 
    LAB_TOK_FLOAT,         //

    LAB_TOK_CHAR,          // ''
    LAB_TOK_STRING,        // ""

    LAB_TOK_LPAREN,        // (
    LAB_TOK_RPAREN,        // )
    LAB_TOK_LBRACKET,      // [
    LAB_TOK_RBRACKET,      // ]
    LAB_TOK_LCURLY,        // {
    LAB_TOK_RCURLY,        // }

    LAB_TOK_COMMA,         // ,
    LAB_TOK_DECIMAL,       // .
    LAB_TOK_COLON,         // :
    LAB_TOK_SEMICOLON,     // ;
    LAB_TOK_DOUBLE_COLON,  // ::
    LAB_TOK_RARROW,        // ->

    LAB_TOK_KW_BOOL,       // bool
    LAB_TOK_KW_INT,        // int
    LAB_TOK_KW_FLOAT,      // float
    LAB_TOK_KW_STR,        // str
    LAB_TOK_KW_STRUCT,     // struct
    LAB_TOK_KW_SELF,       // self
    LAB_TOK_KW_RETURN,     // return
    LAB_TOK_KW_IF,         // if
    LAB_TOK_KW_ELSE,       // else
    LAB_TOK_KW_NIL,        // nil
    LAB_TOK_KW_FOR,        // for
    LAB_TOK_KW_WHILE,      // while
    LAB_TOK_KW_BREAK,      // break
    LAB_TOK_KW_CONTINUE,   // continue
    LAB_TOK_KW_TRUE,       // true
    LAB_TOK_KW_FALSE,      // false
    LAB_TOK_KW_AND,        // and
    LAB_TOK_KW_OR,         // or
    LAB_TOK_KW_NOT,        // not
    

    LAB_TOK_OPERATOR_ADD,          // +
    LAB_TOK_OPERATOR_SUB,          // -
    LAB_TOK_OPERATOR_MUL,          // *
    LAB_TOK_OPERATOR_DIV,          // /
    LAB_TOK_OPERATOR_EQUALS,       // =
    LAB_TOK_OPERATOR_LESST,        // <
    LAB_TOK_OPERATOR_GREATERT,     // >
    LAB_TOK_OPERATOR_LESST_EQU,    // <=
    LAB_TOK_OPERATOR_GREATERT_EQU, // >=
    LAB_TOK_OPERATOR_COMPARE,      // ==
    /*
    LAB_TOK_OPERATOR_NOT,          // !
    LAB_TOK_OPERATOR_BITSHIFTL,    // <<
    LAB_TOK_OPERATOR_BITSHIFTR,    // >>
    LAB_TOK_OPERATOR_BIT_XOR,      // ^
    LAB_TOK_OPERATOR_BIT_AND,      // &
    LAB_TOK_OPERATOR_BIT_OR,       // |
    */

    LAB_TOK_EOF,                   // \0

}   lab_tokens_e_t;