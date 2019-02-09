#pragma once

#include <stdint.h>
#include <lab/logger.h>
#include <lab/vector.h>

typedef enum lab_vm_value_type_e {
    LAB_VM_VALUE_TYPE_NIL,
    LAB_VM_VALUE_TYPE_BOOLEAN,
    LAB_VM_VALUE_TYPE_NUMBER,
} lab_vm_value_type_e_t;

typedef struct lab_vm_value {
    lab_vm_value_type_e_t type;
    union {
        bool   boolean;
        double number;
    } as;
} lab_vm_value_t;

void lab_vm_value_print(lab_vm_value_t value);

#define LAB_VM_VALUE_NIL ((lab_vm_value_t){ .type = LAB_VM_VALUE_TYPE_NIL, { .number = 0 }})
#define LAB_VM_VALUE_BOOL(value) ((lab_vm_value_t){ .type = LAB_VM_VALUE_TYPE_BOOLEAN, { .boolean = value }})
#define LAB_VM_VALUE_NUMBER(value) ((lab_vm_value_t){ .type = LAB_VM_VALUE_TYPE_NUMBER, { .number = value }})

typedef struct lab_vm_bytecode {
    lab_vec_t bytes;
    lab_vec_t lines;
    lab_vec_t constants;
} lab_vm_bytecode_t;

bool lab_vm_bytecode_init(lab_vm_bytecode_t* bytecode, size_t init_size);
void lab_vm_bytecode_free(lab_vm_bytecode_t* bytecode);



typedef enum lab_vm_op_e {
    LAB_VM_OP_CONSTANT,
    LAB_VM_OP_NEGATE,
    LAB_VM_OP_ADD,
    LAB_VM_OP_SUBTRACT,
    LAB_VM_OP_MULTIPLY,
    LAB_VM_OP_DIVIDE,
    LAB_VM_OP_RETURN,
} lab_vm_op_e_t;

bool lab_vm_bytecode_write_byte (lab_vm_bytecode_t* bytecode, int line, uint8_t  byte);
bool lab_vm_bytecode_write_bytes(lab_vm_bytecode_t* bytecode, int line, uint8_t* bytes, size_t count);

size_t lab_vm_bytecode_write_constant(lab_vm_bytecode_t* bytecode, lab_vm_value_t value);

size_t lab_vm_bytecode_dissassemble_instruction(lab_vm_bytecode_t* bytecode, size_t index);
void   lab_vm_bytecode_dissassemble(lab_vm_bytecode_t* bytecode, const char* name);


typedef enum lab_vm_interpret_result_e_t {
    LAB_VM_INTERPRET_RESULT_SUCCESS,
    LAB_VM_INTERPRET_RESULT_COMPILE_ERROR,
    LAB_VM_INTERPRET_RESULT_RUNTIME_ERROR,
} lab_vm_interpret_result_e_t;

typedef struct lab_vm {
    lab_vm_bytecode_t* bytecode;
    uint8_t* ip;
    lab_vec_t stack;
    lab_vm_value_t* stack_top;
} lab_vm_t;

bool lab_vm_init(lab_vm_t* vm);
void lab_vm_free(lab_vm_t* vm);

void lab_vm_reset_stack(lab_vm_t* vm);

void           lab_vm_push(lab_vm_t* vm, lab_vm_value_t value);
lab_vm_value_t lab_vm_pop (lab_vm_t* vm);

lab_vm_interpret_result_e_t lab_vm_interpret_bytecode(lab_vm_t* vm, lab_vm_bytecode_t* bytecode, bool debug_trace);
lab_vm_interpret_result_e_t lab_vm_run(lab_vm_t* vm, bool debug_trace);