#pragma once

#include "vm.h"

char* lab_read_file(const char* file_name, bool binary);
bool lab_lex_and_parse(lab_vm_bytecode_t* bytecode, const char* code, const char* name, bool debug);