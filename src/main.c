#include "lexer.h"
#include "parser.h"
#include "vm.h"

#include <lab/math.h>

#include <stdio.h>
#include <string.h>

#include <time.h>
#include <stdlib.h>

#include <getopt.h>

int main(int argc, char* const argv[]) {

    int opt = 0;

    /*lab_vec_t file_names;
    lab_vec_init(&file_names, sizeof(char*), 8);*/

    char* file_name = NULL;
    char* output_name = NULL;
    bool bytecode_flag = false;
    bool compile_flag = false;

    bool debug_prints = false;

    while((opt = getopt(argc, argv, "f:b:co:d")) != -1) {
        switch(opt) {

            case 'd':
                lab_println("Debug mode enabled");
                debug_prints = true;
                break;

            case 'c':
                lab_println("Compiling source files");
                compile_flag = true;
                break;

            case 'f':
                lab_println("Source file: %s", optarg);
                file_name = malloc(strlen(optarg) + 1);
                memcpy(file_name, optarg, strlen(optarg) + 1);
                bytecode_flag = false;
                break;

            case 'b':
                lab_println("Bytecode file: %s", optarg);
                file_name = malloc(strlen(optarg) + 1);
                memcpy(file_name, optarg, strlen(optarg) + 1);
                bytecode_flag = true;
                break;

            case 'o': 
                lab_println("Outputing to file: %s", optarg);
                output_name = malloc(strlen(optarg) + 1);
                memcpy(output_name, optarg, strlen(optarg) + 1);
                break;

            case '?': 
                lab_errorln("Unknown option %c", optopt);
                free(file_name);
                free(output_name);
                return EXIT_FAILURE;

            default:
                break;
        }
    }

    if(file_name==NULL) {
        lab_errorln("No input files!");
        free(file_name);
        free(output_name);
        return EXIT_FAILURE;
    }

    if(compile_flag) {
        if(bytecode_flag) {
            lab_errorln("Cannot compile bytecode!");
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }
        if(output_name==NULL) {
            lab_errorln("Needs output file '-o'!");
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }
    }

    if(compile_flag) {

        char* file_contents = NULL;

        FILE* file = fopen(file_name, "r");
        if(file == NULL){
            lab_errorln("Failed to open '%s'!", file_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        size_t file_size = 0;

        fseek(file, 0, SEEK_END);
        file_size = ftell(file) + 1;
        rewind(file);

        file_contents = malloc(file_size);
        if(file_contents == NULL) {
            lab_errorln("Failed to allocate buffer for '%s'!", file_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        if(fread(file_contents, 1, file_size, file) < file_size - 1) {
            lab_errorln("Failed to read '%s'!", file_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        file_contents[file_size - 1] = '\0';

        fclose(file);

        lab_lexer_token_container_t tokens;
        lab_parser_t parser;
        lab_vm_bytecode_t bytecode;

        lab_lexer_token_container_init(&tokens, 16);

        lab_lexer_lex(&tokens, file_contents);

        if(debug_prints) lab_lexer_token_container_print(&tokens);

        lab_vm_bytecode_init(&bytecode, 16);
        lab_parser_init(&parser);

        lab_parser_parse(&parser, &tokens, &bytecode);

        size_t serialized_len = 0;
        uint8_t* serialized = lab_vm_bytecode_serialize(&bytecode, &serialized_len, true);

        if(debug_prints) lab_vm_bytecode_dissassemble(&bytecode, file_name);

        if(debug_prints) {
            lab_notice("Serialized ");
            for(size_t i = 24; i < serialized_len; i++) {
                lab_print_raw("%02hhx ", serialized[i]);
            }
            lab_println_raw("");
        }


        FILE* out = fopen(output_name, "wb+");
        if(out == NULL){
            lab_errorln("Failed to open '%s'!", output_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        fwrite(serialized, serialized_len, 1, out);

        fclose(file);

    }

    else if(!bytecode_flag) {



        char* file_contents = NULL;

        FILE* file = fopen(file_name, "r");
        if(file == NULL){
            lab_errorln("Failed to open '%s'!", file_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        size_t file_size = 0;

        fseek(file, 0, SEEK_END);
        file_size = ftell(file) + 1;
        rewind(file);

        file_contents = malloc(file_size);
        if(file_contents == NULL) {
            lab_errorln("Failed to allocate buffer for '%s'!", file_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        if(fread(file_contents, 1, file_size, file) < file_size - 1) {
            lab_errorln("Failed to read '%s'!", file_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        file_contents[file_size - 1] = '\0';

        fclose(file);

        lab_lexer_token_container_t tokens;
        lab_parser_t parser;
        lab_vm_bytecode_t bytecode;

        lab_vm_t vm;
        lab_vm_init(&vm);

        lab_lexer_token_container_init(&tokens, 16);

        lab_lexer_lex(&tokens, file_contents);

        if(debug_prints) lab_lexer_token_container_print(&tokens);

        lab_parser_init(&parser);
        lab_vm_bytecode_init(&bytecode, 16);

        lab_parser_parse(&parser, &tokens, &bytecode);


        if(debug_prints) lab_vm_bytecode_dissassemble(&bytecode, "Bytecode");

        if(!parser.was_error) {
            if(debug_prints) lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);

            if(lab_vm_interpret_bytecode(&vm, &bytecode, debug_prints) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
                lab_errorln("Failed to execute bytecode!");
            }

        } else {
            lab_warnln("There was an error in compiling the input thus it will not be ran on the Virtual Machine");
        }


        lab_lexer_token_container_free(&tokens);
        lab_vm_bytecode_free(&bytecode);

        lab_vm_free(&vm);

    }


    else if(bytecode_flag) {

        uint8_t* file_contents = NULL;

        FILE* file = fopen(file_name, "rb");
        if(file == NULL){
            lab_errorln("Failed to open '%s'!", file_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        size_t file_size = 0;

        fseek(file, 0, SEEK_END);
        file_size = ftell(file);
        rewind(file);

        file_contents = malloc(file_size);
        if(file_contents == NULL) {
            lab_errorln("Failed to allocate buffer for '%s'!", file_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        if(fread(file_contents, 1, file_size, file) < file_size - 1) {
            lab_errorln("Failed to read '%s'!", file_name);
            free(file_name);
            free(output_name);
            return EXIT_FAILURE;
        }

        fclose(file);

        lab_vm_t vm;
        lab_vm_init(&vm);

        lab_vm_bytecode_t bytecode;
        lab_vm_bytecode_init(&bytecode, 0);
        lab_vm_bytecode_deserialize(&bytecode, file_contents);

        if(debug_prints) lab_vm_bytecode_dissassemble(&bytecode, file_name);

        if(debug_prints) lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);

        if(lab_vm_interpret_bytecode(&vm, &bytecode, debug_prints) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
            lab_errorln("Failed to execute bytecode!");
        }

        lab_vm_bytecode_free(&bytecode);
        lab_vm_free(&vm);

    }

    return 0;

}