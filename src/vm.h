#pragma once

#include <stdint.h>
#include <lab/logger.h>
#include <lab/vector.h>

typedef enum lab_vm_object_type_e {
    LAB_VM_OBJECT_STRING
} lab_vm_object_type_e_t;

typedef struct lab_vm_object {
    lab_vm_object_type_e_t type;
} lab_vm_object_t;

typedef enum lab_vm_value_type_e {
    LAB_VM_VALUE_TYPE_NIL,
    LAB_VM_VALUE_TYPE_BOOLEAN,
    LAB_VM_VALUE_TYPE_NUMBER,
    LAB_VM_VALUE_TYPE_OBJECT,
} lab_vm_value_type_e_t;

typedef struct lab_vm_value {
    lab_vm_value_type_e_t type;
    union {
        bool   boolean;
        double number;
        lab_vm_object_t* object;
    } as;
} lab_vm_value_t;

bool lab_vm_value_object_is_type(lab_vm_value_t value, lab_vm_object_type_e_t type);

void lab_vm_value_print    (lab_vm_value_t value);
bool lab_vm_value_is_falsey(lab_vm_value_t value);
bool lab_vm_value_is_equal (lab_vm_value_t a, lab_vm_value_t b);

#define LAB_VM_VALUE_NIL           ((lab_vm_value_t){ .type = LAB_VM_VALUE_TYPE_NIL, { .number = 0 }})
#define LAB_VM_VALUE_BOOL(value)   ((lab_vm_value_t){ .type = LAB_VM_VALUE_TYPE_BOOLEAN, { .boolean = value }})
#define LAB_VM_VALUE_NUMBER(value) ((lab_vm_value_t){ .type = LAB_VM_VALUE_TYPE_NUMBER, { .number = value }})
#define LAB_VM_VALUE_OBJECT(value) ((lab_vm_value_t){ .type = LAB_VM_VALUE_TYPE_OBJECT, { .object = value }})

#define LAB_VM_VALUE_IS_NIL(value)    ((value).type == LAB_VM_VALUE_TYPE_NIL)
#define LAB_VM_VALUE_IS_BOOL(value)   ((value).type == LAB_VM_VALUE_TYPE_BOOLEAN)
#define LAB_VM_VALUE_IS_NUMBER(value) ((value).type == LAB_VM_VALUE_TYPE_NUMBER)
#define LAB_VM_VALUE_IS_OBJ(value)    ((value).type == LAB_VM_VALUE_TYPE_OBJECT) 

#define LAB_VM_VALUE_IS_STRING(value) (lab_vm_value_object_is_type(value, LAB_VM_OBJECT_STRING))


typedef struct lab_vm_obj_string {
    lab_vm_object_t object;
    uint32_t len;
    char* data;
} lab_vm_obj_string_t;

#define LAB_VM_VALUE_AS_STR(value)  ((lab_vm_obj_string*)value.as.object)
#define LAB_VM_VALUE_AS_CSTR(value) (LAB_VM_VALUE_AS_STR(value)->data)






typedef struct lab_vm_bytecode {
    lab_vec_t bytes;
    lab_vec_t lines;
    lab_vec_t constants;
} lab_vm_bytecode_t;

bool lab_vm_bytecode_init(lab_vm_bytecode_t* bytecode, size_t init_size);
void lab_vm_bytecode_free(lab_vm_bytecode_t* bytecode);



typedef enum lab_vm_op_e {
    LAB_VM_OP_CONSTANT,      // Loads a constant value onto the stack, no longer constant once on the stack
    LAB_VM_OP_CONSTANT_2L,   // Same as constant except has a two byte index
    LAB_VM_OP_NIL,           // Loads a nil value onto the stack
    LAB_VM_OP_TRUE,          // Loads a boolean true value onto the stack
    LAB_VM_OP_FALSE,         // Loads a boolean false value onto the stack
    LAB_VM_OP_EQUAL,         // Checks if two values from stack are equal
    LAB_VM_OP_GREATER,       // Checks if the second top value is greater than the top value
    LAB_VM_OP_LESSER,        // Checks if the second top value is lesser than the top value
    LAB_VM_OP_NEGATE,        // Negates a number ( - ) from stack
    LAB_VM_OP_NOT,           // Not operator, ( ! )
    LAB_VM_OP_AND,           // Boolean and operator ( and )
    LAB_VM_OP_ADD,           // Adds two numbers from stack
    LAB_VM_OP_SUBTRACT,      // Subtracts two numbers from stack
    LAB_VM_OP_MULTIPLY,      // Multiplies two numbers from stack
    LAB_VM_OP_DIVIDE,        // Divies two numbers from the stack
    LAB_VM_OP_RETURN,        // Returns the value at stack top and halts the program
} lab_vm_op_e_t;

bool lab_vm_bytecode_write_byte (lab_vm_bytecode_t* bytecode, int line, uint8_t  byte);
bool lab_vm_bytecode_write_bytes(lab_vm_bytecode_t* bytecode, int line, uint8_t* bytes, size_t count);

size_t lab_vm_bytecode_write_constant(lab_vm_bytecode_t* bytecode, lab_vm_value_t value);

size_t lab_vm_bytecode_dissassemble_instruction(lab_vm_bytecode_t* bytecode, size_t index);
void   lab_vm_bytecode_dissassemble(lab_vm_bytecode_t* bytecode, const char* name);

/*
    Serial Format

    Note: lab_vm_value_t has 4 bytes of padding in between 'type' and 'as'

    HEADER - If any value is zero, it is interpreted as not existing
    32 bit unsigned integer: Index of where constant data starts
    32 bit unsigned integer: Length of constant data
    32 bit unsigned integer: Index of where the bytecode starts
    32 bit unsigned integer: Length of bytecode
    32 bit unsigned integer: Index of where the line data starts
    32 bit unsigned integer: Length of line data
    DATA - Starts 192 bits ( 24 bytes ) from the begining
    CONSTANTS
    BYTECODE
    LINE_DATA

*/
uint8_t* lab_vm_bytecode_serialize  (lab_vm_bytecode_t* bytecode, size_t* size, bool include_line_data);
bool     lab_vm_bytecode_deserialize(lab_vm_bytecode_t* bytecode, uint8_t* data);

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
lab_vm_value_t lab_vm_peek(lab_vm_t* vm, int distance);

lab_vm_interpret_result_e_t lab_vm_interpret_bytecode(lab_vm_t* vm, lab_vm_bytecode_t* bytecode, bool debug_trace);
lab_vm_interpret_result_e_t lab_vm_run(lab_vm_t* vm, bool debug_trace);