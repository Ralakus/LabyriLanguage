#include <stdio.h>
#include <stdlib.h>

#include "utilities.h"

#include "lexer.h"
#include "parser.h"
#include "vm.h"

char* lab_read_file(const char* file_name, bool binary) {

    char* file_contents = NULL;

    FILE* file = fopen(file_name, binary ? "rb" : "r");
    if(file == NULL){
        lab_errorln("Failed to open '%s'!", file_name);
        return NULL;
    }

    size_t file_size = 0;

    fseek(file, 0, SEEK_END);
    file_size = ftell(file) + 1;
    rewind(file);

    file_contents = malloc(file_size);
    if(file_contents == NULL) {
        lab_errorln("Failed to allocate buffer for '%s'!", file_name);
        return NULL;
    }

    if(fread(file_contents, 1, file_size, file) < file_size - 1) {
        lab_errorln("Failed to read '%s'!", file_name);
        free(file_contents);
        return NULL;
    }

    file_contents[file_size - 1] = '\0';

    fclose(file);

    return file_contents;
}

bool lab_lex_and_parse(lab_vm_bytecode_t* bytecode, const char* code, const char* name, bool debug) {

    lab_lexer_token_container_t container;
    lab_lexer_token_container_init(&container, 8);

    if(!lab_lexer_lex(&container, code)) {
        lab_errorln("Failed to lex \'%s\'!", name);
        lab_lexer_token_container_free(&container);
        return false;
    }

    if(debug) lab_lexer_token_container_print(&container);

    lab_parser_t parser;
    lab_parser_init(&parser);

    lab_vm_bytecode_init(bytecode, 8);

    if(!lab_parser_parse(&parser, &container, bytecode)) {
        lab_errorln("There was an error compilg the code!");
        lab_lexer_token_container_free(&container);
        lab_vm_bytecode_free(bytecode);
        return false;
    }

    if(debug) lab_vm_bytecode_dissassemble(bytecode, name);

    lab_lexer_token_container_free(&container);
    lab_parser_free(&parser);

    return true;
}