#include "lexer_conf.h"
#include "parser.h"

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

void lab_print_help_base(){
    lab_print_version();
    lab_noticeln("-f or --file <files> to compile files");
    lab_noticeln("-h or --help to print help");
    lab_noticeln("-d or --debug to enable debug printing");
}

_Noreturn void lab_print_help() {
    lab_print_help_base();
    exit(0);
}

_Noreturn void lab_print_help_err() {
    lab_print_help_base();
    exit(1);
}

int main(int argc, char* argv[]) {

    clock_t start, end;
    double total_time, arg_convert_time, arg_parse_time, file_read_time, lex_time, arg_free_time, file_free_time;

    start = clock();

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

    end = clock();

    arg_convert_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();

    lab_vec_t file_names;
    lab_vec_init(&file_names, sizeof(lab_vec_t), 0);

    // Parse arguments

    enum lab_parse_state_e {

        lab_parse_state_other,
        lab_parse_state_files,

    }   parse_state = lab_parse_state_other;

    bool print_tokens = false;

    for(size_t i = 0; i < lab_vec_size(&args); i++) {
        if(strcmp("-f", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0 || strcmp("--file", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0) {
            parse_state = lab_parse_state_files;
            continue;
        }

        else if(strcmp("-d", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0 || strcmp("--debug", (char*)((lab_vec_t*)lab_vec_at(&args, i))->raw_data)==0) {
            print_tokens = true;
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

    end = clock();

    arg_parse_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();

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

        for(size_t i = 0; i < lab_vec_size(&args); i++) {  // Free arguments vector
            lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
        }
        lab_vec_free(&args);

        lab_print_help_err();
        return 1;
    }

    end = clock();

    file_read_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();

    lab_lexer_token_container_t tokens;

    for(size_t i = 0; i < lab_vec_size(&file_contents); i++) {
        lab_lexer_token_container_init(&tokens);
        lab_custom_lexer_lex(
            &tokens,
            (char*)((lab_vec_t*)lab_vec_at(&file_contents, i))->raw_data,
            lab_vec_size((lab_vec_t*)lab_vec_at(&file_contents, i)),
            NULL
        );

        if(print_tokens) {
            lab_noticeln("Tokens for file: \"%s\"", (char*)((lab_vec_t*)lab_vec_at(&file_names, i))->raw_data);
            for(size_t j = 0; j < tokens.count; j++) {
                char* tok_str = tok_to_string((lab_tokens_e)tokens.tokens[j].id);
                lab_println("Token: %s: %s at line: %d, column: %d", (const char*)tok_str, tokens.tokens[j].data, tokens.tokens[j].line, tokens.tokens[j].column);
                free(tok_str);
            }
            lab_noticeln("END");
        } else {
            lab_noticeln("Token debug printing off");
        }

        lab_lexer_token_container_free(&tokens);

    }

    end = clock();

    lex_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();

    for(size_t i = 0; i < lab_vec_size(&args); i++) {       // Free arguments vector
        lab_vec_free((lab_vec_t*)lab_vec_at(&args, i));
    }
    lab_vec_free(&args);

    end = clock();

    arg_free_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();

    for(size_t i = 0; i < lab_vec_size(&file_names); i++) {  // Free file name vector
        lab_vec_free((lab_vec_t*)lab_vec_at(&file_names, i));
    }
    lab_vec_free(&file_names);

    for(size_t i = 0; i < lab_vec_size(&file_contents); i++) {  // Free file content vector
        lab_vec_free((lab_vec_t*)lab_vec_at(&file_contents, i));
    }
    lab_vec_free(&file_contents);

    end = clock(); 

    file_free_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    total_time = arg_convert_time + arg_parse_time + file_read_time + lex_time + arg_free_time + file_free_time;

    lab_successln("Argument convert time: %fms", arg_convert_time * 1000);
    lab_successln("Argument parse time: %fms",   arg_parse_time   * 1000);
    lab_successln("File read time: %fms",        file_read_time   * 1000);
    lab_successln("Lex time: %fms",              lex_time         * 1000);
    lab_successln("Argument free time: %fms",    arg_free_time    * 1000);
    lab_successln("File free time: %fms",        file_free_time   * 1000);
    lab_successln("Total time: %fms",            total_time       * 1000);

    return 0;
}