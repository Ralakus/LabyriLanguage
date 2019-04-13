#include <lab/arg_parser.h>
#include <lab/math.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "vm.h"

#include "utilities.h"
#include "repl.h"

bool compile_file(const char* input_file, const char* output_file, bool debug);

int main(int argc, const char* argv[]) {

    const char* file_name   = NULL;
    const char* output_name = NULL;

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

    if(arg_debug.found)    { lab_noticeln("Debug mode enabled");     }
    if(arg_compile.found)  { lab_noticeln("Compiling source files"); }
    if(arg_bytecode.found) { lab_noticeln("Bytecode mode");          }

    if(arg_bytecode.found && arg_compile.found) {
        lab_arg_free(&arg_debug);
        lab_arg_free(&arg_compile);
        lab_arg_free(&arg_files);
        lab_arg_free(&arg_bytecode);
        lab_arg_free(&arg_output);
        lab_arg_free(&arg_repl);

        lab_errorln("Cannot compile bytecode!");
    }

    if(arg_output.found) {
        lab_println("Outputing to file: %s", *(const char**)lab_vec_at(&arg_output.preceeding_args, 0));
        output_name = LAB_VEC_TYPE_AT(&arg_output.preceeding_args, 0, const char*);
    }

    if(arg_files.found) {
        lab_println("Input file: %s", *(const char**)lab_vec_at(&arg_files.preceeding_args, 0));
        file_name = LAB_VEC_TYPE_AT(&arg_files.preceeding_args, 0, const char*);
    }

    lab_arg_free(&arg_debug);
    lab_arg_free(&arg_compile);
    lab_arg_free(&arg_files);
    lab_arg_free(&arg_bytecode);
    lab_arg_free(&arg_output);
    lab_arg_free(&arg_repl);

    if(arg_repl.found) {
        if(!lab_repl(arg_debug.found)) return EXIT_FAILURE;
        else return EXIT_SUCCESS;
    }

    if(file_name==NULL) {
        lab_errorln("No input files!");
        return EXIT_FAILURE;
    }

    if(arg_compile.found) {
        if(!compile_file(file_name, output_name, arg_debug.found)) return EXIT_FAILURE;
        else return EXIT_SUCCESS;
    }

    lab_vm_bytecode_t bytecode;

    bool was_error = false;

    if(arg_bytecode.found) {

        uint8_t* serialized = lab_read_file(file_name, true);
        if(serialized == NULL) {
            return EXIT_FAILURE;
        }
        if(!lab_vm_bytecode_deserialize(&bytecode, serialized)) {
            lab_errorln("Failed to deserialize bytecode from file \'%s\'!", file_name);
            was_error = true;
        }
        free(serialized);

        if(arg_debug.found) lab_vm_bytecode_dissassemble(&bytecode, file_name);


    } else {

        char* code = lab_read_file(file_name, false);
        if(code == NULL) {
            return EXIT_FAILURE;
        }
        if(!lab_lex_and_parse(&bytecode, code, file_name, arg_debug.found)) {
            was_error = true;
        }
        free(code);

    }

    lab_vm_t vm;
    lab_vm_init(&vm);

    if(!was_error) {
        if(arg_debug.found) lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);

        if(lab_vm_interpret_bytecode(&vm, &bytecode, arg_debug.found) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
            lab_errorln("Failed to execute bytecode!");
        }

    } else {
        lab_warnln("There was an error in compiling the input thus it will not be ran on the Virtual Machine");
    }

    lab_vm_bytecode_free(&bytecode);
    lab_vm_free(&vm);
    

    return EXIT_SUCCESS;

}

bool compile_file(const char* input_file, const char* output_file, bool debug) {

    char* code = lab_read_file(input_file, false);

    lab_vm_bytecode_t bytecode;

    if(!lab_lex_and_parse(&bytecode, code, input_file, debug)) {
        return false;
    }

    size_t   serialized_len;
    uint8_t* serialized = lab_vm_bytecode_serialize(&bytecode, &serialized_len, debug);

    if(debug) {
        lab_notice("Serialized ");
        for(size_t i = 0; i < serialized_len; i++) {
            lab_print_raw("%02hhx ", serialized[i]);
        }
        lab_println_raw("");
    }

    lab_vm_bytecode_free(&bytecode);

    FILE* out = fopen(output_file, "wb+");
    if(out == NULL){
        lab_errorln("Failed to open '%s'!", output_file);
        return false;
    }

    fwrite(serialized, serialized_len, 1, out);
    fclose(out);

    free(serialized);

    return true;
}