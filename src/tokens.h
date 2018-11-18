#pragma once

typedef enum lab_tokens_e {
    lab_tok_err,

    lab_tok_identifier,
    lab_tok_number,

    lab_tok_char,
    lab_tok_string,

    lab_tok_lparen,
    lab_tok_rparen,
    lab_tok_lbracket,
    lab_tok_rbracket,
    lab_tok_lcurly,
    lab_tok_rcurly,

    lab_tok_comma,
    lab_tok_decimal,
    lab_tok_colon,
    lab_tok_semicolon,
    lab_tok_double_colon,
    lab_tok_rarrow,

    lab_tok_kw_var,
    lab_tok_kw_struct,
    lab_tok_kw_self,
    lab_tok_kw_return,
    lab_tok_kw_if,
    lab_tok_kw_else,
    lab_tok_kw_nil,
    lab_tok_kw_for,
    lab_tok_kw_while,
    lab_tok_kw_break,
    lab_tok_kw_continue,
    lab_tok_kw_true,
    lab_tok_kw_false,
    

    lab_tok_operator_add,
    lab_tok_operator_sub,
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

    lab_tok_operator_concat,

    lab_tok_eof,

}   lab_tokens_e;