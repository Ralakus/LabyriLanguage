#include "parser.h"

#include <stdlib.h>

void lab_parser_init(lab_parser_t* parser) {
    parser->container  = NULL;
    parser->current    = NULL;
    parser->bytecode   = NULL;
    parser->was_error  = false;
    parser->panic_mode = false;
}

void lab_parser_free(lab_parser_t* parser) {
    parser->container  = NULL;
    parser->current    = NULL;
    parser->bytecode   = NULL;
    parser->was_error  = false;
    parser->panic_mode = false;
}







static void error_at(lab_parser_t* parser, lab_lexer_token_t* token, const char* message) {
    if(parser->panic_mode) { return; }
    parser->panic_mode = true;

    lab_error("(Line: %d, Column: %d): Error", token->line, token->column);

    if(token->type == LAB_TOK_EOF) {
        lab_error_raw(" at end");
    } else if(token->type == LAB_TOK_ERR) {
        // Nothing
    } else {
        if(token->type >= LAB_TOK_COMMENT && token->type <= LAB_TOK_STRING) {
            lab_error_raw(" at \"%.*s\"", token->data_len, token->data);
        } else {
            lab_error_raw(" at %d \"%s\"", token->type, lab_token_to_string_lookup[token->type]);
        }
    }

    lab_errorln_raw(": %s", message);
    parser->was_error = true;
}

static void error(lab_parser_t* parser, const char* message) {
    error_at(parser, (parser->current - 1), message);
}

static void error_at_current(lab_parser_t* parser, const char* message) {
    error_at(parser, parser->current, message);
}

static void advance(lab_parser_t* parser) {
    ++parser->current;

    if(parser->current->type == LAB_TOK_ERR && (parser->current - (lab_lexer_token_t*)lab_vec_at(&parser->container->tokens, 0) < lab_vec_size(&parser->container->tokens))) {
        error_at_current(parser, parser->current->data);
    }
}

static void consume(lab_parser_t* parser, lab_tokens_e_t token_type, const char* message) {
    if(parser->current->type == token_type) {
        advance(parser);
        return;
    } else {
        error_at_current(parser, message);
    }
}

static bool emit_byte(lab_parser_t* parser, uint8_t byte) {
    return lab_vm_bytecode_write_byte(parser->bytecode, (parser->current - 1)->line, byte);
}

static bool emit_bytes(lab_parser_t* parser, uint8_t byte1, uint8_t byte2) {
    return emit_byte(parser, byte1) && emit_byte(parser, byte2);
}

static bool emit_return(lab_parser_t* parser) {
    return emit_byte(parser, LAB_VM_OP_RETURN);
}

static short make_constant(lab_parser_t* parser, lab_vm_value_t value) {
    int constant = lab_vm_bytecode_write_constant(parser->bytecode, value);
    if(constant > UINT16_MAX) {
        error(parser, "Too many constants in on chunk of bytecode, exceeded 65535 constants");
        return 0;
    }

    return (short)constant;
}

static bool emit_constant(lab_parser_t* parser, lab_vm_value_t value) {
    short index = make_constant(parser, value);
    if(index <= UINT8_MAX) {
        return emit_bytes(parser, LAB_VM_OP_CONSTANT, make_constant(parser, value));
    } else {
        return emit_byte(parser, LAB_VM_OP_CONSTANT_2L) &&
               emit_bytes(parser, ((uint8_t*)&index)[0], ((uint8_t*)&index)[1]);
    }
}

typedef enum precedence_e {                  
  PREC_NONE,
  PREC_ASSIGNMENT,  // =
  PREC_OR,          // or
  PREC_AND,         // and
  PREC_UNARY,       // not -
  PREC_EQUALITY,    // == !=
  PREC_COMPARISON,  // < > <= >=
  PREC_TERM,        // + -
  PREC_FACTOR,      // * /
  PREC_CALL,        // . () []
  PREC_PRIMARY
} precedence_e_t;

typedef void(*parse_fn_t)(lab_parser_t*);

typedef struct parse_rule {
    parse_fn_t prefix;
    parse_fn_t infix;
    precedence_e_t precedence;
} parse_rule_t;

static void number(lab_parser_t* parser);
static void literal(lab_parser_t* parser);
static void expression(lab_parser_t* parser);
static void grouping(lab_parser_t* parser);
static void unary(lab_parser_t* parser); 
static void binary(lab_parser_t* parser);
static void parse_precedence(lab_parser_t* parser, precedence_e_t precedence);

parse_rule_t rules[49] = {
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_ERR
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_COMMENT
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_IDENTIFIER
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_INTEGER
    { number,   NULL,   PREC_NONE       },  // LAB_TOK_FLOAT
    { number,   NULL,   PREC_NONE       },  // LAB_TOK_CHAR
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_STRING
    { grouping, NULL,   PREC_CALL       },  // LAB_TOK_LPAREN
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_RPAREN
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_LBRACKET
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_RBRACKET
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_LCURLY
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_RCURLY
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_COMMA
    { NULL,     NULL,   PREC_CALL       },  // LAB_TOK_DECIMAL
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_COLON
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_SEMICOLON
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_DOUBLE_COLON,
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_RARROW
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_BOOL
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_INT
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_FLOAT
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_STR
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_STRUCT
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_SELF
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_RETURN
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_IF
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_ELSE
    { literal,  NULL,   PREC_NONE       },  // LAB_TOK_KW_NIL
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_FOR
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_WHILE
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_BREAK
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_KW_CONTINUE
    { literal,  NULL,   PREC_NONE       },  // LAB_TOK_KW_TRUE
    { literal,  NULL,   PREC_NONE       },  // LAB_TOK_KW_FALSE
    { NULL,     NULL,   PREC_AND        },  // LAB_TOK_KW_AND
    { NULL,     NULL,   PREC_OR         },  // LAB_TOK_KW_OR
    { unary,    NULL,   PREC_NONE       },  // LAB_TOK_KW_NOT,    
    { NULL,     binary, PREC_TERM       },  // LAB_TOK_OPERATOR_ADD
    { unary,    binary, PREC_TERM       },  // LAB_TOK_OPERATOR_SUB
    { NULL,     binary, PREC_FACTOR     },  // LAB_TOK_OPERATOR_MUL
    { NULL,     binary, PREC_FACTOR     },  // LAB_TOK_OPERATOR_DIV
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_OPERATOR_EQUALS
    { NULL,     binary, PREC_COMPARISON },  // LAB_TOK_OPERATOR_LESST
    { NULL,     binary, PREC_COMPARISON },  // LAB_TOK_OPERATOR_GREATERT
    { NULL,     binary, PREC_COMPARISON },  // LAB_TOK_OPERATOR_LESST_EQU
    { NULL,     binary, PREC_COMPARISON },  // LAB_TOK_OPERATOR_GREATERT_EQU
    { NULL,     binary, PREC_EQUALITY   },  // LAB_TOK_OPERATOR_COMPARE
    /*
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_OPERATOR_NOT
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_OPERATOR_BITSHIFTL
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_OPERATOR_BITSHIFTR
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_OPERATOR_BIT_XOR
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_OPERATOR_BIT_AND
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_OPERATOR_BIT_OR
    */
    { NULL,     NULL,   PREC_NONE       },  // LAB_TOK_EOF
};

static parse_rule_t* get_rule(lab_tokens_e_t type) {
    return &rules[type];
}

static void parse_precedence(lab_parser_t* parser, precedence_e_t precedence) {
    advance(parser);
    parse_fn_t prefix_rule = get_rule((parser->current - 1)->type)->prefix;
    if(prefix_rule == NULL) {
        error(parser, "Expect expression");
        return;
    } else {

        prefix_rule(parser);

        while(precedence <= get_rule(parser->current->type)->precedence) {
            advance(parser);
            parse_fn_t infix_rule = get_rule((parser->current - 1)->type)->infix;
            infix_rule(parser);
        }
    }
}

static void number(lab_parser_t* parser) {
    double value = strtod((parser->current - 1)->data, NULL);
    emit_constant(parser, LAB_VM_VALUE_NUMBER(value));
}

static void literal(lab_parser_t* parser) {
    switch((parser->current - 1)->type) {
        case LAB_TOK_KW_TRUE:
            emit_byte(parser, LAB_VM_OP_TRUE);
            break;
        case LAB_TOK_KW_FALSE:
            emit_byte(parser, LAB_VM_OP_FALSE);
            break;
        case LAB_TOK_KW_NIL:
            emit_byte(parser, LAB_VM_OP_NIL);
            break;
        default:
            return;
    }
}

static void expression(lab_parser_t* parser) {
    parse_precedence(parser, PREC_ASSIGNMENT);
}

static void grouping(lab_parser_t* parser) {
    expression(parser);
    consume(parser, LAB_TOK_RPAREN, "Expect \')\' after expression");
}

static void unary(lab_parser_t* parser) {
    lab_tokens_e_t operator_type = (parser->current - 1)->type;

    parse_precedence(parser, PREC_UNARY);

    switch (operator_type)
    {
        case LAB_TOK_OPERATOR_SUB: 
            emit_byte(parser, LAB_VM_OP_NEGATE);
            break;
        case LAB_TOK_KW_NOT:
            emit_byte(parser, LAB_VM_OP_NOT);
            break;
        default:
            break;
    }
}

static void binary(lab_parser_t* parser) {
    lab_tokens_e_t operator_type = (parser->current - 1)->type;

    
    parse_rule_t* rule = get_rule(operator_type);                 
    parse_precedence(parser, (precedence_e_t)(rule->precedence + 1));

    switch (operator_type)
    {
        case LAB_TOK_OPERATOR_ADD:
            emit_byte(parser, LAB_VM_OP_ADD);
            break;
        case LAB_TOK_OPERATOR_SUB:
            emit_byte(parser, LAB_VM_OP_SUBTRACT);
            break;
        case LAB_TOK_OPERATOR_MUL:
            emit_byte(parser, LAB_VM_OP_MULTIPLY);
            break;
        case LAB_TOK_OPERATOR_DIV:
            emit_byte(parser, LAB_VM_OP_DIVIDE);
            break;

        case LAB_TOK_OPERATOR_LESST: 
            emit_byte(parser, LAB_VM_OP_LESSER);
            break;
        case LAB_TOK_OPERATOR_GREATERT:
            emit_byte(parser, LAB_VM_OP_GREATER);
            break;
        case LAB_TOK_OPERATOR_LESST_EQU:
            emit_bytes(parser, LAB_VM_OP_GREATER, LAB_VM_OP_NOT);
            break;
        case LAB_TOK_OPERATOR_GREATERT_EQU:
            emit_bytes(parser, LAB_VM_OP_LESSER, LAB_VM_OP_NOT);
            break;
        case LAB_TOK_OPERATOR_COMPARE:
            emit_byte(parser, LAB_VM_OP_EQUAL);
            break;
    
        default:
            break;
    }
}








bool lab_parser_parse(lab_parser_t* parser, lab_lexer_token_container_t* tokens, lab_vm_bytecode_t* bytecode) {
    parser->container = tokens;
    parser->bytecode = bytecode;

    parser->current = (lab_lexer_token_t*)lab_vec_at(&tokens->tokens, 0);
    if(parser->current->type == LAB_TOK_ERR) {
        error_at_current(parser, parser->current->data);
    }


    parser->was_error  = false;
    parser->panic_mode = false;

    expression(parser);
    consume(parser, LAB_TOK_EOF, "Expected end of expression!");
    emit_return(parser);

    /*if(!parser->was_error) {
        lab_vm_bytecode_dissassemble(parser->bytecode, "Bytecode");
    }*/

    return !parser->was_error;
}