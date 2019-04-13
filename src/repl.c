#include "repl.h"

#include <lab/math.h>

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#include "lexer.h"
#include "parser.h"
#include "vm.h"

#include "utilities.h"

bool lab_repl(bool debug_mode) {

    lab_noticeln("Type \':help\' for help");

    lab_lexer_token_container_t tokens;
    lab_parser_t                parser;
    lab_vm_bytecode_t           bytecode;
    lab_vm_t                    vm;
    lab_vm_init(&vm);

    bool command_exec = false;

    char line[65536];
    for(;;) {

        command_exec = false;

        lab_print("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            lab_print_raw("\n");
            break;
        }


        lab_lexer_token_container_init(&tokens, 16);
        lab_lexer_lex(&tokens, line);

        if(((lab_lexer_token_t*)lab_vec_at(&tokens.tokens, 0))->type == LAB_TOK_COLON) {
            if(lab_vec_len(&tokens.tokens) > 2) {

                if(((lab_lexer_token_t*)lab_vec_at(&tokens.tokens, 1))->type == LAB_TOK_IDENTIFIER) {

                    #define TOK_AT_INDEX(i) ((lab_lexer_token_t*)lab_vec_at(&tokens.tokens, i))
                    #define COMMAND_EQ(str) (memcmp(TOK_AT_INDEX(1)->data, str, min(sizeof(str) - 1, TOK_AT_INDEX(1)->data_len))==0)

                    if(COMMAND_EQ("exit")) {
                        lab_lexer_token_container_free(&tokens);
                        break;
                    } else if(COMMAND_EQ("debug")) {
                        debug_mode = !debug_mode;
                        if(debug_mode) lab_noticeln("Debug mode enabled"); else lab_noticeln("Debug mode disabled");
                    } else if(COMMAND_EQ("help")) {
                        lab_noticeln("\':\' must be before every command");
                        lab_noticeln("\'help\' prints this message");
                        lab_noticeln("\'debug\' toggles debug mode");
                        lab_noticeln("\'sys\' executes command on system");
                        lab_noticeln("\'loadf\' loads and runs a Labyri script from file");
                        lab_noticeln("\'loadb\' loads and runs Labyri bytecode from file");
                        lab_noticeln("\'exit\' exits repl");
                    } else if(COMMAND_EQ("sys")) {
                        if(lab_vec_len(&tokens.tokens) > 3) {
                            if(system(TOK_AT_INDEX(2)->data)) {
                                lab_errorln("Invalid command: \'%s\'", TOK_AT_INDEX(2)->data);
                            }
                        } else {
                            lab_errorln("Expected argument after sys!");
                        }
                    } else if(COMMAND_EQ("loadf")) {

                        if(lab_vec_len(&tokens.tokens) > 3) {

                            bool was_error = false;

                            char* file_name = malloc((TOK_AT_INDEX(lab_vec_len(&tokens.tokens) - 1)->data - 1 - TOK_AT_INDEX(2)->data) + 1);
                            if(file_name==NULL) { 
                                lab_errorln("Failed to allocate file_name buffer!");
                                break;
                            }
                            if(memcpy(file_name, TOK_AT_INDEX(2)->data, (TOK_AT_INDEX(lab_vec_len(&tokens.tokens) - 1)->data - 1 - TOK_AT_INDEX(2)->data))==NULL) {
                                lab_errorln("Failed to copy file_name buffer!");
                                break;
                            }
                            file_name[(TOK_AT_INDEX(lab_vec_len(&tokens.tokens) - 1)->data - 1 - TOK_AT_INDEX(2)->data)] = '\0';

                            char* code = lab_read_file(file_name, false);
                            if(code == NULL) {
                                was_error = true;
                            }
                            
                            if(!was_error)
                            if(!lab_lex_and_parse(&bytecode, code, TOK_AT_INDEX(2)->data, debug_mode)) {
                                was_error = true;
                            }
                            free(code);

                            if(debug_mode) lab_vm_bytecode_dissassemble(&bytecode, file_name);

                            free(file_name);

                            if(!was_error) {
                                if(debug_mode) lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);

                                if(lab_vm_interpret_bytecode(&vm, &bytecode, debug_mode) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
                                    lab_errorln("Failed to execute bytecode!");
                                }

                                lab_vm_bytecode_free(&bytecode);

                            } else {
                                lab_warnln("There was an error in compiling the input thus it will not be ran on the Virtual Machine");
                            }

                        } else {
                            lab_errorln("Expected argument after loadf!");
                        }

                    } else if(COMMAND_EQ("loadb")) {

                        if(lab_vec_len(&tokens.tokens) > 3) {

                            bool was_error = false;

                            char* file_name = malloc((TOK_AT_INDEX(lab_vec_len(&tokens.tokens) - 1)->data - 1 - TOK_AT_INDEX(2)->data) + 1);
                            if(file_name==NULL) { 
                                lab_errorln("Failed to allocate file_name buffer!");
                                break;
                            }
                            if(memcpy(file_name, TOK_AT_INDEX(2)->data, (TOK_AT_INDEX(lab_vec_len(&tokens.tokens) - 1)->data - 1 - TOK_AT_INDEX(2)->data))==NULL) {
                                lab_errorln("Failed to copy file_name buffer!");
                                break;
                            }
                            file_name[(TOK_AT_INDEX(lab_vec_len(&tokens.tokens) - 1)->data - 1 - TOK_AT_INDEX(2)->data)] = '\0';

                            uint8_t* serialized = lab_read_file(file_name, true);
                            if(serialized == NULL) {
                                was_error = true;
                            }

                            lab_vm_bytecode_init(&bytecode, 8);
                            if(!was_error)
                            if(!lab_vm_bytecode_deserialize(&bytecode, serialized)) {
                                lab_errorln("Failed to deserialize bytecode from file \'%.*s\'!", (TOK_AT_INDEX(lab_vec_len(&tokens.tokens) - 1)->data - TOK_AT_INDEX(2)->data), TOK_AT_INDEX(2)->data);
                                was_error = true;
                            }
                            free(serialized);

                            if(debug_mode) lab_vm_bytecode_dissassemble(&bytecode, file_name);

                            free(file_name);

                            if(!was_error) {
                                if(debug_mode) lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);

                                if(lab_vm_interpret_bytecode(&vm, &bytecode, debug_mode) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
                                    lab_errorln("Failed to execute bytecode!");
                                }

                                lab_vm_bytecode_free(&bytecode);

                            } else {
                                lab_warnln("There was an error in compiling the input thus it will not be ran on the Virtual Machine");
                            }
                        } else {
                            lab_errorln("Expected argument after loadb!");
                        }

                    } else {
                        lab_errorln("\'%.*s\' is not a valid command", TOK_AT_INDEX(1)->data_len, TOK_AT_INDEX(1)->data);
                    }
                }

            } else {
                lab_errorln("REPL expects command after \':\'");
            }
            command_exec = true;
        }

        if(!command_exec) {

            if(debug_mode) lab_lexer_token_container_print(&tokens);

            lab_parser_init(&parser);
            lab_vm_bytecode_init(&bytecode, 16);

            lab_parser_parse(&parser, &tokens, &bytecode);

            if(debug_mode) lab_vm_bytecode_dissassemble(&bytecode, "REPL Bytecode");

            if(!parser.was_error) {
                if(debug_mode) lab_noticeln(LAB_ANSI_COLOR_CYAN"--== Virtual Machine Stack Trace ==--"LAB_ANSI_COLOR_RESET);


                if(lab_vm_interpret_bytecode(&vm, &bytecode, debug_mode) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
                    lab_errorln("Failed to execute bytecode!");
                }

            } else {
                lab_warnln("There was an error in compiling the input thus it will not be ran on the Virtual Machine");
            }


            lab_vm_bytecode_free(&bytecode);

        }

        lab_lexer_token_container_free(&tokens);

    }

    lab_vm_free(&vm);

    return true;

}