#pragma once

typedef enum tokens_e {
    tok_nil,

    tok_whitespace_space,
    tok_whitespace_tab,
    tok_whitespace_return,
    tok_whitespace_newline,

    tok_identifier,
    tok_number,

    tok_char,
    tok_string,

    tok_lparen,
    tok_rparen,
    tok_lbracket,
    tok_rbracket,
    tok_lcurley,
    tok_rcurley,

    tok_comma,
    tok_colon,
    tok_semicolon,
    tok_comment,

    tok_kw_func,
    tok_kw_let,
    tok_kw_return,

    tok_operator_plus,
    tok_operator_minus,
    tok_operator_mul,
    tok_operator_div,
    tok_operator_equals,
    tok_operator_xor,
    tok_operator_and,
    tok_operator_lesst,
    tok_operator_greatert,
    tok_operator_or,
    tok_operator_not,
    tok_operator_bitshiftl,
    tok_operator_bitshiftr,

    tok_eof,

}   tokens_e;