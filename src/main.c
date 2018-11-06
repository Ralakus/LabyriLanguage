#include "lexer_conf.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

#include "parser.h"

int main(int argc, char* argv[]) {
    clock_t start, end;
    double lex_read_files_time, lex_time, lex_file_free_time, lex_total_time;

    size_t file_count           = argc - 1;
    char** file_names           = NULL;
    size_t* file_name_sizes     = NULL;
    char** file_contents        = NULL;
    size_t* file_contents_sizes = NULL;

    start = clock();

    if(argc > 1) {
        FILE* cur_file = NULL;
        file_names          = (char**)malloc(sizeof(char*)  * file_count);
        file_name_sizes     = (size_t*)malloc(sizeof(size_t) * file_count);
        file_contents       = (char**)malloc(sizeof(char*)  * file_count);
        file_contents_sizes = (size_t*)malloc(sizeof(size_t) * file_count);

        if(file_names==NULL) {
            lab_errorln("Failed to allocate file name buffer!");
            free(file_names);
            free(file_name_sizes);
            free(file_contents);
            free(file_contents_sizes);
            return 1;
        } else if(file_name_sizes==NULL) {
            lab_errorln("Failed to allocate file name size buffer!");
            free(file_names);
            free(file_name_sizes);
            free(file_contents);
            free(file_contents_sizes);
            return 1;
        } else if(file_contents==NULL) {
            lab_errorln("Failed to allocate file contents buffer!");
            free(file_names);
            free(file_name_sizes);
            free(file_contents);
            free(file_contents_sizes);
            return 1;
        } else if(file_contents_sizes==NULL) {
            lab_errorln("Failed to allocate file contents size buffer!");
            free(file_names);
            free(file_name_sizes);
            free(file_contents);
            free(file_contents_sizes);
            return 1;
        }

        for(int i = 1; i < argc; i++) {

            file_name_sizes[i-1] = strlen(argv[i])+1;
            file_names[i-1] = (char*)malloc(file_name_sizes[i-1]);
            if(file_names[i-1]==NULL) {
                lab_errorln("Failed to allocate file name for file: \"%s\"!", argv[i]);
                for(int j = 0; j < file_count; j++) {
                    free(file_names[j]);
                    free(file_contents[j]);
                }
                free(file_names);
                free(file_name_sizes);
                free(file_contents);
                free(file_contents_sizes);
                return 1;
            }

            file_names[i-1][file_name_sizes[i-1]-1] = '\0';
            memcpy(file_names[i-1], argv[i], file_name_sizes[i-1]-1);

            cur_file = fopen(argv[i], "r");

            if(cur_file==NULL) {
                lab_errorln("Failed to openfile: \"%s\"!", argv[i]);
                for(int j = 0; j < file_count; j++) {
                    free(file_names[j]);
                    free(file_contents[j]);
                }
                free(file_names);
                free(file_name_sizes);
                free(file_contents);
                free(file_contents_sizes);
                return 1;
            }

            fseek(cur_file, 0, SEEK_END);
            file_contents_sizes[i-1] = ftell(cur_file)+1;
            fseek(cur_file, 0, SEEK_SET);
            file_contents[i-1] = (char*)malloc(file_contents_sizes[i-1]);

            if(file_contents[i-1]==NULL) {
                lab_errorln("Failed to allocate file buffer for file: \"%s\"!", argv[i]);
                for(int j = 0; j < file_count; j++) {
                    free(file_names[j]);
                    free(file_contents[j]);
                }
                free(file_names);
                free(file_name_sizes);
                free(file_contents);
                free(file_contents_sizes);
                return 1;
            }

            file_contents[i-1][file_contents_sizes[i-1]-1] = '\0';

            fread(file_contents[i-1], 1, file_contents_sizes[i-1], cur_file);
            fclose(cur_file);
        }
    }
    else {
        lab_errorln("No input files!");
        return 1;
    }

    end = clock();

    lex_read_files_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    lab_lexer_token_container_t tokens;

    start = clock();

    for(size_t i = 0; i < file_count; i++) {
        lab_lexer_token_container_init(&tokens);
        lab_custom_lexer_lex(&tokens, file_contents[i], file_contents_sizes[i], NULL);

        lab_noticeln("Tokens for file: \"%s\"", file_names[i]);
        for(size_t j = 0; j < tokens.count; j++) {
            char* tok_str = tok_to_string((lab_tokens_e)tokens.tokens[j].id);
            lab_println("Token: %s: %s", (const char*)tok_str, tokens.tokens[j].data);
            free(tok_str);
        }
        lab_noticeln("END");

        lab_lexer_token_container_free(&tokens);
    }

    end = clock();

    lex_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    start = clock();

    for(int j = 0; j < file_count; j++) {
        free(file_names[j]);
        free(file_contents[j]);
    }
    free(file_names);
    free(file_name_sizes);
    free(file_contents);
    free(file_contents_sizes);

    end  = clock();

    lex_file_free_time = ((double)(end - start)) / CLOCKS_PER_SEC;

    lex_total_time = lex_read_files_time + lex_time + lex_file_free_time;

    lab_successln("Read files time %fms", lex_read_files_time * 1000);
    lab_successln("Lex time %fms", lex_time * 1000);
    lab_successln("File free time %fms", lex_file_free_time * 1000);
    lab_successln("Total time: %fms", lex_total_time * 1000);

    return 0;
}