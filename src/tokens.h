#pragma once

typedef enum lab_tokens_e {
    lab_tok_err,

    lab_tok_whitespace_space,
    lab_tok_whitespace_tab,
    lab_tok_whitespace_return,
    lab_tok_whitespace_newline,

    lab_tok_identifier,
    lab_tok_number,

    lab_tok_char,
    lab_tok_string,

    lab_tok_lparen,
    lab_tok_rparen,
    lab_tok_lbracket,
    lab_tok_rbracket,
    lab_tok_lcurley,
    lab_tok_rcurley,

    lab_tok_comma,
    lab_tok_colon,
    lab_tok_semicolon,
    lab_tok_comment,

    lab_tok_kw_func,
    lab_tok_kw_let,
    lab_tok_kw_return,
    lab_tok_kw_as,
    lab_tok_kw_if,
    lab_tok_kw_else,
    lab_tok_kw_nil,
    lab_tok_kw_for,
    lab_tok_kw_while,
    lab_tok_kw_true,
    lab_tok_kw_false,
    

    lab_tok_operator_plus,
    lab_tok_operator_minus,
    lab_tok_operator_mul,
    lab_tok_operator_div,
    lab_tok_operator_equals,
    lab_tok_operator_xor,
    lab_tok_operator_and,
    lab_tok_operator_lesst,
    lab_tok_operator_greatert,
    lab_tok_operator_or,
    lab_tok_operator_not,
    lab_tok_operator_bitshiftl,
    lab_tok_operator_bitshiftr,

    lab_tok_eof,

}   lab_tokens_e;