
#include "arg_parser.h"

#include <lab/math.h>

#include <string.h>

void lab_arg_init(lab_arg_t* arg, const char* short_name, const char* long_name, const char* description,  bool expect_preceeding) {

    arg->short_name  = short_name;
    arg->long_name   = long_name;
    arg->description = description;

    arg->expect_preceeding = expect_preceeding;

    arg->found = false;
    lab_vec_init(&arg->preceeding_args, sizeof(const char*), expect_preceeding ? 1 : 0);

}

void lab_arg_free(lab_arg_t* arg) {
    lab_vec_free(&arg->preceeding_args);
}



void lab_arg_parser_init   (lab_arg_parser_t* parser) {
    lab_vec_init(&parser->args, sizeof(lab_arg_t*), 8);
    lab_vec_init(&parser->extra_args, sizeof(const char*), 0);
}

void lab_arg_parser_add_arg(lab_arg_parser_t* parser, lab_arg_t* arg) {
    lab_vec_push_back(&parser->args, &arg);
}

bool lab_arg_parser_parse  (lab_arg_parser_t* parser, int argc, const char** argv) {

    lab_arg_t* prev_arg = NULL;

    for(int i = 1; i < argc; i++) {

        if(argv[i][0] == '-') {

            bool arg_found = false;

            if(argv[i][1] == '-')  {

                for(size_t j = 0; j < lab_vec_size(&parser->args); j++) {

                    lab_arg_t* cur_arg = *(lab_arg_t**)lab_vec_at(&parser->args, j);

                    if(strcmp(cur_arg->long_name, &argv[i][2])==0) {
                        cur_arg->found = true;
                        prev_arg = cur_arg;
                        arg_found = true;
                        break;
                    }

                }

            } else {

                size_t arg_len = strlen(argv[i]);
                
                for(size_t j = 0; j < lab_vec_size(&parser->args); j++) {

                    lab_arg_t* cur_arg = *(lab_arg_t**)lab_vec_at(&parser->args, j);

                    for(size_t k = 1; k < arg_len; k++) {

                        if(
                            memcmp(cur_arg->short_name, &argv[i][k],
                            min(
                            strlen(cur_arg->short_name), strlen(&argv[i][k])
                            ))==0) {
                            cur_arg->found = true;
                            prev_arg = cur_arg;
                            arg_found = true;
                            break;
                        }

                    }
                }

            }

            if(!arg_found) {
                lab_errorln("Unexpected argument: '%s'!", argv[i]);
                return false;
            }

        } else { // Preceeding arg

            if(prev_arg != NULL) {
                
                if(prev_arg->expect_preceeding) {
                    lab_vec_push_back(&prev_arg->preceeding_args, &argv[i]);
                } else {
                    lab_errorln("Argument '%s' ( '%s' ) does not expect preceeding arguments!", prev_arg->long_name, prev_arg->short_name);
                    return false;
                }

            } else {

                lab_vec_push_back(&parser->extra_args, &argv[i]);

            }

        }

    }

    for(size_t i = 0; i < lab_vec_size(&parser->args); i++) {
        lab_arg_t* cur_arg = *(lab_arg_t**)lab_vec_at(&parser->args, i);

        if(cur_arg->expect_preceeding && lab_vec_size(&cur_arg->preceeding_args) < 1 && cur_arg->found) {
            lab_errorln("Argument '%s' ( '%s' ) expects preceeding arguments!", cur_arg->long_name, cur_arg->short_name);
            return false;
        }
    }

    return true;

}

void lab_arg_parser_free   (lab_arg_parser_t* parser) {
    lab_vec_free(&parser->args);
    lab_vec_free(&parser->extra_args);
}