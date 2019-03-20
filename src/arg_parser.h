#pragma once

#include <lab/vector.h>

typedef struct lab_arg {

    const char* short_name;
    const char* long_name;
    const char* description;
    
    bool expect_preceeding; // If false, throws error if a preceeding arg is found

    bool found;
    lab_vec_t preceeding_args; // const char*

} lab_arg_t;

void lab_arg_init(lab_arg_t* arg, const char* short_name, const char* long_name, const char* description, bool expect_preceeding);
void lab_arg_free(lab_arg_t* arg);

typedef struct lab_arg_parser {

    lab_vec_t args; // lab_arg_t*
    lab_vec_t extra_args; // const char*

} lab_arg_parser_t;

void lab_arg_parser_init   (lab_arg_parser_t* parser);
void lab_arg_parser_add_arg(lab_arg_parser_t* parser, lab_arg_t* arg); // Does not delete pointer
bool lab_arg_parser_parse  (lab_arg_parser_t* parser, int argc, const char** argv);
void lab_arg_parser_free   (lab_arg_parser_t* parser); 