#include "parser.h"

void lab_parser_init(lab_parser_t* parser) {
    parser->container = NULL;
    parser->current = NULL;
    parser->index = 0;
    parser->bytecode = NULL;
}

void lab_parser_free(lab_parser_t* parser) {

}