#include "lexer_conf.h"

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

#include <stdbool.h>

typedef struct lab_vec_t {
    
    size_t type_size;
    size_t alloc_size;
    size_t used_size;
    void* raw_data;

} lab_vec_t;

bool lab_vec_init(lab_vec_t* vec, size_t type_size, size_t init_size) {
    vec->type_size  = type_size;
    vec->used_size  = 0;
    vec->alloc_size = init_size;
    vec->raw_data   = malloc(type_size * vec->alloc_size);
    if(vec->raw_data == NULL) {
        lab_errorln("Failed to allocate vector array with size of %d and type size of %d", vec->alloc_size, vec->type_size);
        return false;
    } else {
        return true;
    }
}

size_t lab_vec_size(lab_vec_t* vec) {
    return vec->used_size;
}

size_t lab_vec_alloc_size(lab_vec_t* vec) {
    return vec->alloc_size;
}

size_t lab_vec_type_size(lab_vec_t* vec) {
    return vec->type_size;
}

void* lab_vec_at(lab_vec_t* vec, size_t index) {
    if(index >= vec->used_size) {
        lab_errorln("Tried to access data outside of vector of size %d but access index of %d", vec->used_size, index);
        return NULL;
    } else {
        return vec->raw_data + (vec->type_size * index);
    }
}

bool lab_vec_resize(lab_vec_t* vec, size_t new_size) {
    vec->raw_data = realloc(vec->raw_data, vec->type_size * new_size);
    if(vec->raw_data == NULL) {
        lab_errorln("Failed to reallocate vector from size %d to %d with type size of %d", vec->alloc_size, vec->type_size * new_size, vec->type_size);
        return false;
    } else {
        vec->alloc_size = vec->type_size * new_size;
        if(vec->used_size > vec->alloc_size) {
            vec->used_size = vec->alloc_size;
        }
        return true;
    }
}

void* lab_vec_push_back(lab_vec_t* vec, void* raw_data) {
    if(vec->used_size >= vec->alloc_size) {
        if(!lab_vec_resize(vec, vec->alloc_size + 1)) {
            lab_errorln("Failed to push back vector!");
            return NULL;
        }
    }
    if(memcpy(lab_vec_at(vec, vec->used_size + 1), raw_data, vec->type_size)==NULL) {
        lab_errorln("Failed to copy data into vector!");
        return NULL;
    } else {
        ++vec->used_size;
        return lab_vec_at(vec, vec->used_size-1);
    }
}


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
        /*for(size_t j = 0; j < tokens.count; j++) {
            char* tok_str = tok_to_string((lab_tokens_e)tokens.tokens[j].id);
            lab_println("Token: %s: %s", (const char*)tok_str, tokens.tokens[j].data);
            free(tok_str);
        }*/
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