#include "lexer_conf.h"

#include <lab/vector.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

#define LAB_VERSION_NUMBER "0.1.0"

void lab_print_version() {
    lab_noticeln("Labyrinth Compiler Version: %s", LAB_VERSION_NUMBER);
}

_Noreturn void lab_print_help() {
    lab_print_version();
    lab_noticeln("-f or --file <files> to compile files");
    lab_noticeln("-h or --help to print help");
    exit(0);
}

_Noreturn void lab_print_help_err() {
    lab_print_version();
    lab_noticeln("-f or --file <files> to compile files");
    lab_noticeln("-h or --help to print help");
    exit(1);
}

int main(int argc, char* argv[]) {

    clock_t start, end;
    double lex_read_files_time, lex_time, lex_file_free_time, lex_total_time;

    lab_vec_t args;
    lab_vec_init(&args, sizeof(lab_vec_t), argc - 1);

    // Read arguments into vector
    
    for(int i = 1; i < argc; i++) {
        lab_vec_t temp;
        size_t arg_size = strlen(argv[i]) + 1; // +1 to include null termination
        if(!lab_vec_init(&temp, sizeof(char), arg_size)) {
            lab_errorln("Failed to init argument vector!");
            return 1;
        }
        if(lab_vec_push_back_arr(&temp, argv[i], arg_size)==NULL) {
            lab_errorln("Failed to push back argument in argument vector!");

            for(size_t i = 0; i < lab_vec_size(&args); i++) {  // Free arguments vector
                lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
            }
            lab_vec_free(&args);

            return 1;
        }
        lab_vec_push_back(&args, &temp);
    }

    lab_vec_t file_names;
    lab_vec_init(&file_names, sizeof(lab_vec_t), 0);

    // Parse arguments

    enum lab_parse_state_e {

        lab_parse_state_other,
        lab_parse_state_files,

    }   parse_state = lab_parse_state_other;

    for(size_t i = 0; i < lab_vec_size(&args); i++) {
        if(strcmp("-f", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0 || strcmp("--file", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0) {
            parse_state = lab_parse_state_files;
            continue;
        }

        else if(strcmp("-h", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0 || strcmp("--help", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0){

            for(size_t i = 0; i < lab_vec_size(&args); i++) {  // Free arguments vector
                lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
            }
            lab_vec_free(&args);

            for(size_t i = 0; i < lab_vec_size(&file_names); i++) {  // Free file name vector
                lab_vec_free((lab_vec_t*)lab_vec_at(&file_names, i));
            }
            lab_vec_free(&file_names);

            lab_print_help();
        }

        else if(strcmp("-v", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0 || strcmp("--version", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0) {
            lab_print_version();

            for(size_t i = 0; i < lab_vec_size(&args); i++) {  // Free arguments vector
                lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
            }
            lab_vec_free(&args);

            for(size_t i = 0; i < lab_vec_size(&file_names); i++) {  // Free file name vector
                lab_vec_free((lab_vec_t*)lab_vec_at(&file_names, i));
            }
            lab_vec_free(&file_names);

            return 0;
        }

        else if(((char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)[0] == '-') {

            lab_errorln("Invalid argument: \"%s\"!", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data);

            for(size_t i = 0; i < lab_vec_size(&args); i++) {  // Free arguments vector
                lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
            }
            lab_vec_free(&args);

            for(size_t i = 0; i < lab_vec_size(&file_names); i++) {  // Free file name vector
                lab_vec_free((lab_vec_t*)lab_vec_at(&file_names, i));
            }
            lab_vec_free(&file_names);

            lab_print_help_err();
        }

        if(parse_state == lab_parse_state_files) {
            lab_vec_t temp;
            size_t arg_size = strlen((char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data) + 1; // +1 to include null termination
            if(!lab_vec_init(&temp, sizeof(char), arg_size)) {
                lab_errorln("Failed to init argument vector!");
                return 1;
            }
            if(lab_vec_push_back_arr(&temp, (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data, arg_size)==NULL) {
                lab_errorln("Failed to push back argument in argument vector!");

                for(size_t i = 0; i < lab_vec_size(&args); i++) {  // Free arguments vector
                    lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
                }
                lab_vec_free(&args);

                for(size_t i = 0; i < lab_vec_size(&file_names); i++) {  // Free file name vector
                    lab_vec_free((lab_vec_t*)lab_vec_at(&file_names, i));
                }
                lab_vec_free(&file_names);

                return 1;
            }
            lab_vec_push_back(&file_names, &temp);
        }
    }

    lab_vec_t file_contents;
    lab_vec_init(&file_contents, sizeof(lab_vec_t), lab_vec_size(&file_names));

    // Read files

    if(lab_vec_size(&file_names) > 0) {
        FILE* cur_file = NULL;

        for(size_t i = 0; i < lab_vec_size(&file_names); i++) {
            
            cur_file = fopen((char*)((lab_vec_t*)lab_vec_at(&file_names, i))->raw_data, "r");

            if(cur_file==NULL) {
                lab_errorln("Failed to open file: \"%s\"!", (char*)((lab_vec_t*)lab_vec_at(&file_names, i))->raw_data);

                for(size_t i = 0; i < lab_vec_size(&args); i++) {       // Free arguments vector
                    lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
                }
                lab_vec_free(&args);

                for(size_t i = 0; i < lab_vec_size(&file_names); i++) {  // Free file name vector
                    lab_vec_free((lab_vec_t*)lab_vec_at(&file_names, i));
                }
                lab_vec_free(&file_names);

                for(size_t i = 0; i < lab_vec_size(&file_contents); i++) {  // Free file content vector
                    lab_vec_free((lab_vec_t*)lab_vec_at(&file_contents, i));
                }
                lab_vec_free(&file_contents);

                return 1;

            }

            fseek(cur_file, 0, SEEK_END);
            size_t file_size = ftell(cur_file) + 1;
            lab_vec_t temp;
            lab_vec_init(&temp, sizeof(char), file_size);
            if(!lab_vec_init(&temp, sizeof(char), file_size)) {
                lab_errorln("Failed to allocate file content buffer for file: \"%s\"!", (char*)((lab_vec_t*)lab_vec_at(&file_names, i))->raw_data);

                for(size_t i = 0; i < lab_vec_size(&args); i++) {       // Free arguments vector
                    lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
                }
                lab_vec_free(&args);

                for(size_t i = 0; i < lab_vec_size(&file_names); i++) {  // Free file name vector
                    lab_vec_free((lab_vec_t*)lab_vec_at(&file_names, i));
                }
                lab_vec_free(&file_names);

                for(size_t i = 0; i < lab_vec_size(&file_contents); i++) {  // Free file content vector
                    lab_vec_free((lab_vec_t*)lab_vec_at(&file_contents, i));
                }
                lab_vec_free(&file_contents);

                return 1;

            }
            fseek(cur_file, 0, SEEK_SET);

            temp.used_size = temp.alloc_size;
            fread(temp.raw_data, 1, lab_vec_size(&temp), cur_file);

            lab_vec_push_back(&file_contents, &temp);

            *(char*)lab_vec_at((lab_vec_t*)lab_vec_at(&file_contents, i), file_size - 1) = '\0';

            fclose(cur_file);

        }

    } else {
        lab_errorln("No input files!");
        lab_print_help_err();
        return 1;
    }

    lab_lexer_token_container_t tokens;

    for(size_t i = 0; i < lab_vec_size(&file_contents); i++) {
        lab_lexer_token_container_init(&tokens);
        lab_custom_lexer_lex(
            &tokens,
            (char*)((lab_vec_t*)lab_vec_at(&file_contents, i))->raw_data,
            lab_vec_size((lab_vec_t*)lab_vec_at(&file_contents, i)),
            NULL
        );

        lab_noticeln("Tokens for file: \"%s\"", (char*)((lab_vec_t*)lab_vec_at(&file_names, i))->raw_data);
        for(size_t j = 0; j < tokens.count; j++) {
            char* tok_str = tok_to_string((lab_tokens_e)tokens.tokens[j].id);
            lab_println("Token: %s: %s", (const char*)tok_str, tokens.tokens[j].data);
            free(tok_str);
        }
        lab_noticeln("END");

        lab_lexer_token_container_free(&tokens);

    }


    for(size_t i = 0; i < lab_vec_size(&args); i++) {       // Free arguments vector
        lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
    }
    lab_vec_free(&args);

    for(size_t i = 0; i < lab_vec_size(&file_names); i++) {  // Free file name vector
        lab_vec_free((lab_vec_t*)lab_vec_at(&file_names, i));
    }
    lab_vec_free(&file_names);

    for(size_t i = 0; i < lab_vec_size(&file_contents); i++) {  // Free file content vector
        lab_vec_free((lab_vec_t*)lab_vec_at(&file_contents, i));
    }
    lab_vec_free(&file_contents);

    return 0;
}