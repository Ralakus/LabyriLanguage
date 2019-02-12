#include "lexer.h"
#include "parser.h"
#include "vm.h"

#include <stdio.h>
#include <string.h>

 #define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

int main(int argc, const char* argv[]) {

    /*lab_lexer_token_container_t tokens;
    lab_lexer_token_container_init(&tokens, 16);
    
    char code[] = "x: str = \"Hello world!\"; # Test variable\nprint(x + \"\\n\");";
    char code2[] = "-(14.0 + 0.48)";

    lab_lexer_lex(&tokens, code2);

    lab_lexer_token_container_print(&tokens);

    lab_parser_t parser;
    lab_parser_init(&parser);

    lab_vm_bytecode_t bytecode;
    lab_vm_bytecode_init(&bytecode, 8);

    lab_parser_parse(&parser, &tokens, &bytecode);

    lab_lexer_token_container_free(&tokens);

    lab_vm_bytecode_free(&bytecode);*/

    lab_lexer_token_container_t tokens;
    lab_parser_t parser;
    lab_vm_bytecode_t bytecode;

    lab_vm_t vm;
    lab_vm_init(&vm);

    char line[1024];
    for(;;) {
        lab_print("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            lab_print_raw("\n");
            break;
        }
        lab_lexer_token_container_init(&tokens, 16);
        lab_lexer_lex(&tokens, line);
        if(((lab_lexer_token_t*)lab_vec_at(&tokens.tokens, 0))->type == LAB_TOK_IDENTIFIER) {
            if(memcmp(((lab_lexer_token_t*)lab_vec_at(&tokens.tokens, 0))->data, "exit", min(4, ((lab_lexer_token_t*)lab_vec_at(&tokens.tokens, 0))->data_len))==0) {
                lab_lexer_token_container_free(&tokens);
                break;
            }
        }
        lab_lexer_token_container_print(&tokens);

        lab_parser_init(&parser);
        lab_vm_bytecode_init(&bytecode, 16);

        lab_parser_parse(&parser, &tokens, &bytecode);

        lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);
        if(lab_vm_interpret_bytecode(&vm, &bytecode, true) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
            lab_errorln("Failed to execute bytecode!");
        }

        lab_lexer_token_container_free(&tokens);
        lab_vm_bytecode_free(&bytecode);
    }

    lab_vm_free(&vm);

    /*lab_vm_t vm;
    lab_vm_init(&vm);

    lab_vm_bytecode_t bytecode;
    lab_vm_bytecode_init(&bytecode, 8);

    // OP_CONSTANT number 14.0
    size_t constant = lab_vm_bytecode_write_constant(&bytecode, LAB_VM_VALUE_NUMBER(14.0));
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_CONSTANT);
    lab_vm_bytecode_write_byte(&bytecode, 0, constant);

    // OP_CONSTANT number 0.48
    constant = lab_vm_bytecode_write_constant(&bytecode, LAB_VM_VALUE_NUMBER(0.48));
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_CONSTANT);
    lab_vm_bytecode_write_byte(&bytecode, 0, constant);

    // OP_ADD
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_ADD);

    // OP_NEGATE
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_NEGATE);

    // OP_RETURN
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_RETURN);

    lab_vm_bytecode_dissassemble(&bytecode, "Bytecode");

    lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Run Test ==--"LAB_ANSI_COLOR_RESET);

    if(lab_vm_interpret_bytecode(&vm, &bytecode, true) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
        lab_errorln("Failed to execute bytecode!");
    }

    lab_vm_free(&vm);

    lab_vm_bytecode_free(&bytecode);*/

    return 0;

}