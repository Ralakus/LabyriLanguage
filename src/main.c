#include "lexer.h"
#include "parser.h"
#include "vm.h"

#include <lab/math.h>

#include <stdio.h>
#include <string.h>

#include <time.h>
#include <stdlib.h>

int main(int argc, const char* argv[]) {

    clock_t start, end;
    double lex_time = 0.0, parse_time = 0.0, run_time = 0.0;

    lab_lexer_token_container_t tokens;
    lab_parser_t parser;
    lab_vm_bytecode_t bytecode;

    lab_vm_t vm;
    lab_vm_init(&vm);

    /*char* test_file = NULL;

    FILE* file = fopen("../test/test1mil.lab", "r");
    if(file == NULL){
        lab_errorln("Failed to open test file!");
        return EXIT_FAILURE;
    }

    size_t file_size = 0;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file) + 1;
    rewind(file);

    test_file = malloc(file_size);
    if(test_file == NULL) {
        lab_errorln("Failed to allocate!");
        return EXIT_FAILURE;
    }

    if(fread(test_file, 1, file_size, file) < file_size - 1) {
        lab_errorln("Failed to read!");
        return EXIT_FAILURE;
    }

    test_file[file_size - 1] = '\0';

    fclose(file);*/

    char line[65536];
    for(;;) {
        lab_print("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            lab_print_raw("\n");
            break;
        }
        lab_lexer_token_container_init(&tokens, 16);
        start = clock();
        lab_lexer_lex(&tokens, line);
        if(((lab_lexer_token_t*)lab_vec_at(&tokens.tokens, 0))->type == LAB_TOK_IDENTIFIER) {
            if(memcmp(((lab_lexer_token_t*)lab_vec_at(&tokens.tokens, 0))->data, "exit", min(4, ((lab_lexer_token_t*)lab_vec_at(&tokens.tokens, 0))->data_len))==0) {
                lab_lexer_token_container_free(&tokens);
                break;
            }
        }
        end = clock();

        lex_time = ((double)(end - start)) / CLOCKS_PER_SEC;

        lab_lexer_token_container_print(&tokens);

        start = clock();

        lab_parser_init(&parser);
        lab_vm_bytecode_init(&bytecode, 16);

        lab_parser_parse(&parser, &tokens, &bytecode);

        size_t serialized_len = 0;
        uint8_t* serialized = lab_vm_bytecode_serialize(&bytecode, &serialized_len);

        lab_notice("Serialized ");
        for(size_t i = 6; i < serialized_len; i++) {
            lab_print_raw("%02hhx ", serialized[i]);
        }
        lab_println_raw("");

        lab_vm_bytecode_deserialize(&bytecode, serialized);

        lab_noticeln("Deserialzed");

        free(serialized);

        end = clock();

        parse_time = ((double)(end - start)) / CLOCKS_PER_SEC;

        lab_vm_bytecode_dissassemble(&bytecode, "Bytecode");

        if(!parser.was_error) {
            lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);

            start = clock();

            if(lab_vm_interpret_bytecode(&vm, &bytecode, true) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
                lab_errorln("Failed to execute bytecode!");
            }

            end = clock();
        } else {
            lab_warnln("There was an error in compiling the input thus it will not be ran on the Virtual Machine");
        }


        lab_lexer_token_container_free(&tokens);
        lab_vm_bytecode_free(&bytecode);

        lab_successln("Lex time: %fms, Parse time: %fms, Run time: %fms", lex_time * 1000, parse_time * 1000, run_time * 1000);

    }

    lab_vm_free(&vm);

    return 0;

}