#include "lexer.h"
#include "parser.h"
#include "vm.h"

#include <lab/math.h>

#include <stdio.h>
#include <string.h>

#include <time.h>
#include <stdlib.h>

#include <lab/arg_parser.h>

int main(int argc, const char* argv[]) {

    char* file_name = NULL;
    char* output_name = NULL;
    bool bytecode_flag = false;
    bool compile_flag = false;

    bool debug_prints = false;

    lab_arg_parser_t arg_parser;
    lab_arg_parser_init(&arg_parser);


    lab_arg_t arg_debug;
    lab_arg_init(&arg_debug, "d", "debug", "Enables debug printout from lexer, parser, and virtual machine", false);
    lab_arg_parser_add_arg(&arg_parser, &arg_debug);

    lab_arg_t arg_compile;
    lab_arg_init(&arg_compile, "c", "compile", "Compiles source files into bytecode files", false);
    lab_arg_parser_add_arg(&arg_parser, &arg_compile);

    lab_arg_t arg_bytecode;
    lab_arg_init(&arg_bytecode, "b", "bytecode", "Runs bytecode from input, does not lex or parse", false);
    lab_arg_parser_add_arg(&arg_parser, &arg_bytecode);
    
    lab_arg_t arg_files;
    lab_arg_init(&arg_files, "f", "file", "Input files", true);
    lab_arg_parser_add_arg(&arg_parser, &arg_files);

    lab_arg_t arg_output;
    lab_arg_init(&arg_output, "o", "output", "Outputs to file", true);
    lab_arg_parser_add_arg(&arg_parser, &arg_output);

    lab_arg_t arg_repl;
    lab_arg_init(&arg_repl, "r", "repl", "Starts up in repl environment", false);
    lab_arg_parser_add_arg(&arg_parser, &arg_repl);

    if(!lab_arg_parser_parse(&arg_parser, argc, argv)) {
        lab_errorln("Error parsing arguments!");
        return EXIT_FAILURE;
    }

    lab_arg_parser_free(&arg_parser);

    if(arg_debug.found)    { debug_prints  = true; lab_println("Debug mode enabled");     }
    if(arg_compile.found)  { compile_flag  = true; lab_println("Compiling source files"); }
    if(arg_bytecode.found) { bytecode_flag = true; lab_println("Bytecode mode"); }

    if(arg_output.found) {
        lab_println("Outputing to file: %s", *(const char**)lab_vec_at(&arg_output.preceeding_args, 0));
        output_name = malloc(strlen(*(const char**)lab_vec_at(&arg_output.preceeding_args, 0)) + 1);
        memcpy(output_name, *(const char**)lab_vec_at(&arg_output.preceeding_args, 0), strlen(*(const char**)lab_vec_at(&arg_output.preceeding_args, 0)) + 1);
    }

    if(arg_files.found) {
        lab_println("Input file: %s", *(const char**)lab_vec_at(&arg_files.preceeding_args, 0));
        file_name = malloc(strlen(*(const char**)lab_vec_at(&arg_files.preceeding_args, 0)) + 1);
        memcpy(file_name, *(const char**)lab_vec_at(&arg_files.preceeding_args, 0), strlen(*(const char**)lab_vec_at(&arg_files.preceeding_args, 0)) + 1);
    }

    lab_arg_free(&arg_debug);
    lab_arg_free(&arg_compile);
    lab_arg_free(&arg_files);
    lab_arg_free(&arg_bytecode);
    lab_arg_free(&arg_output);
    lab_arg_free(&arg_repl);

    if(arg_repl.found) {

        lab_lexer_token_container_t tokens;
        lab_parser_t                parser;
        lab_vm_bytecode_t           bytecode;
        lab_vm_t                    vm;
        lab_vm_init(&vm);

        char line[65536];
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

            if(debug_prints) lab_lexer_token_container_print(&tokens);

            lab_parser_init(&parser);
            lab_vm_bytecode_init(&bytecode, 16);

            lab_parser_parse(&parser, &tokens, &bytecode);

            if(debug_prints) lab_vm_bytecode_dissassemble(&bytecode, "REPL Bytecode");

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

        }

        lab_vm_free(&vm);

        free(file_name);
        free(output_name);

        return EXIT_SUCCESS;
        
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

        free(file_contents);

        size_t serialized_len = 0;
        uint8_t* serialized = lab_vm_bytecode_serialize(&bytecode, &serialized_len, debug_prints);

        if(debug_prints) lab_vm_bytecode_dissassemble(&bytecode, file_name);

        if(debug_prints) {
            lab_notice("Serialized ");
            for(size_t i = 0; i < serialized_len; i++) {
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
            free(file_contents);
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

        free(file_contents);

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

        free(file_contents);

        if(debug_prints) lab_vm_bytecode_dissassemble(&bytecode, file_name);

        if(debug_prints) lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);

        if(lab_vm_interpret_bytecode(&vm, &bytecode, debug_prints) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
            lab_errorln("Failed to execute bytecode!");
        }

        lab_vm_bytecode_free(&bytecode);
        lab_vm_free(&vm);

    }

    free(file_name);
    free(output_name);

    return 0;

}