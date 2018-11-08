#include "lexer_conf.h"

#include <lab/vector.h>

#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>

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
        return 1;
    }

    for(size_t i = 0; i < lab_vec_size(&file_contents); i++) {
        lab_successln("%s :\n %s", 
        (char*)((lab_vec_t*)lab_vec_at(&file_names, i))->raw_data,
        (char*)((lab_vec_t*)lab_vec_at(&file_contents, i))->raw_data);
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