#include "lexer.h"
#include "parser.h"
#include "vm.h"

#include <lab/math.h>

#include <stdio.h>
#include <string.h>

int main(int argc, const char* argv[]) {

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

        lab_vm_bytecode_dissassemble(&bytecode, "Bytecode");

        if(!parser.was_error) {
            lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);
            if(lab_vm_interpret_bytecode(&vm, &bytecode, true) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
                lab_errorln("Failed to execute bytecode!");
            }
        } else {
            lab_warnln("There was an error in compiling the input thus it will not be ran on the Virtual Machine");
        }


        lab_lexer_token_container_free(&tokens);
        lab_vm_bytecode_free(&bytecode);
    }

    lab_vm_free(&vm);

    return 0;

}