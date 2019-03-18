#include "vm.h"

#include <stdarg.h>
#include <stdio.h>

void lab_vm_value_print(lab_vm_value_t value) {
    switch (value.type) {
        case LAB_VM_VALUE_TYPE_NIL: 
            lab_print_raw("nil");
        break;

        case LAB_VM_VALUE_TYPE_BOOLEAN:
            lab_print_raw(value.as.boolean ? "true" : "false");
        break;

        case LAB_VM_VALUE_TYPE_NUMBER:
            lab_print_raw("%g", value.as.number);
        break;
    }
}

bool lab_vm_value_is_falsey(lab_vm_value_t value) {
    return LAB_VM_VALUE_IS_NIL(value) || (LAB_VM_VALUE_IS_BOOL(value) && !value.as.boolean);
}

bool lab_vm_value_is_equal(lab_vm_value_t a, lab_vm_value_t b) {
    if(a.type == b.type) {
        switch(a.type) {
            case LAB_VM_VALUE_TYPE_NIL:
                return true;
            case LAB_VM_VALUE_TYPE_BOOLEAN: 
                return a.as.boolean == b.as.boolean;
            case LAB_VM_VALUE_TYPE_NUMBER:
                return a.as.number == b.as.number;
        }
    } else {
        return false;
    }
}

bool lab_vm_bytecode_init(lab_vm_bytecode_t* bytecode, size_t init_size) {
    return lab_vec_init(&bytecode->bytes,     sizeof(uint8_t),          init_size) &&
           lab_vec_init(&bytecode->lines,     sizeof(int),              init_size) &&
           lab_vec_init(&bytecode->constants, sizeof(lab_vm_value_t), 0);
}

void lab_vm_bytecode_free(lab_vm_bytecode_t* bytecode) {
    lab_vec_free(&bytecode->bytes), lab_vec_free(&bytecode->lines), lab_vec_free(&bytecode->constants); 
}

bool lab_vm_bytecode_write_byte(lab_vm_bytecode_t* bytecode, int line, uint8_t byte) {
    return lab_vm_bytecode_write_bytes(bytecode, line, &byte, 1);
}

bool lab_vm_bytecode_write_bytes(lab_vm_bytecode_t* bytecode, int line, uint8_t* bytes, size_t count) {
    if((lab_vec_push_back_arr(&bytecode->bytes, bytes, count)==NULL) || (lab_vec_push_back(&bytecode->lines, &line)==NULL)) {
        return false;
    } else {
        return true;
    }
}

size_t lab_vm_bytecode_write_constant(lab_vm_bytecode_t* bytecode, lab_vm_value_t value) {
    if(lab_vec_push_back(&bytecode->constants, &value)==NULL) {
        return -1;
    } else {
        return lab_vec_size(&bytecode->constants) - 1;
    }
}

void lab_vm_bytecode_dissassemble(lab_vm_bytecode_t* bytecode, const char* name) {
    lab_noticeln(LAB_ANSI_COLOR_CYAN"--== %s ==--"LAB_ANSI_COLOR_RESET, name);

    lab_noticeln(LAB_ANSI_COLOR_CYAN"- Constants -"LAB_ANSI_COLOR_RESET);
    for (size_t i = 0; i <lab_vec_size(&bytecode->constants); i++) {
        lab_print(LAB_ANSI_COLOR_RED"%04d      "LAB_ANSI_COLOR_YELLOW, i);
        lab_vm_value_print(*(lab_vm_value_t*)lab_vec_at(&bytecode->constants, i));
        lab_println_raw(LAB_ANSI_COLOR_RESET);
    }

    lab_noticeln(LAB_ANSI_COLOR_CYAN"- Instructions -"LAB_ANSI_COLOR_RESET);

    for(size_t i = 0; i < lab_vec_size(&bytecode->bytes);) {
        i = lab_vm_bytecode_dissassemble_instruction(bytecode, i);
    }

}

static size_t lab_vm_bytecode_dissassemble_instruction_simple(const char* name, size_t index) {
    lab_print_raw(LAB_ANSI_COLOR_GREEN"%s\n"LAB_ANSI_COLOR_RESET, name);
    return index + 1;
}

static size_t lab_vm_bytecode_dissassemble_instruction_constant(const char* name, lab_vm_bytecode_t* bytecode, size_t index) {
    uint8_t constant = *(uint8_t*)lab_vec_at(&bytecode->bytes, index + 1);
    lab_print_raw(LAB_ANSI_COLOR_GREEN"%-16s "LAB_ANSI_COLOR_YELLOW"%4d \'", name, constant);
    lab_vm_value_print(*(lab_vm_value_t*)lab_vec_at(&bytecode->constants, constant));
    lab_print_raw("\'\n"LAB_ANSI_COLOR_RESET);
    return index + 2;
}

static size_t lab_vm_bytecode_dissassemble_instruction_constant_2_long(const char* name, lab_vm_bytecode_t* bytecode, size_t index) {
    short constant = *(short*)lab_vec_at(&bytecode->bytes, index + 1);
    lab_print_raw(LAB_ANSI_COLOR_GREEN"%-16s "LAB_ANSI_COLOR_YELLOW"%4d \'", name, constant);
    lab_vm_value_print(*(lab_vm_value_t*)lab_vec_at(&bytecode->constants, constant));
    lab_print_raw("\'\n"LAB_ANSI_COLOR_RESET);
    return index + 3;
}

size_t lab_vm_bytecode_dissassemble_instruction(lab_vm_bytecode_t* bytecode, size_t index) {
    lab_print(LAB_ANSI_COLOR_RED"%04d ", index);

    if(index > 0) {
        if(*(int*)lab_vec_at(&bytecode->lines, index) == *(int*)lab_vec_at(&bytecode->lines, index - 1)) {
            lab_print_raw("   | "LAB_ANSI_COLOR_RESET);
        } else {
            lab_print_raw("%4d "LAB_ANSI_COLOR_RESET, *(int*)lab_vec_at(&bytecode->lines, index));
        }
    } else {
        lab_print_raw("%4d "LAB_ANSI_COLOR_RESET, *(int*)lab_vec_at(&bytecode->lines, index));
    }

    uint8_t instruction = *(uint8_t*)lab_vec_at(&bytecode->bytes, index);
    switch (instruction) {
        case LAB_VM_OP_CONSTANT: {
            return lab_vm_bytecode_dissassemble_instruction_constant("constant", bytecode, index);
        }

        case LAB_VM_OP_CONSTANT_2L: {
            return lab_vm_bytecode_dissassemble_instruction_constant_2_long("constant long", bytecode, index);
        }

        case LAB_VM_OP_TRUE: {
            return lab_vm_bytecode_dissassemble_instruction_simple("boolean true", index);
        }

        case LAB_VM_OP_FALSE: {
            return lab_vm_bytecode_dissassemble_instruction_simple("boolean false", index);
        }

        case LAB_VM_OP_NIL: {
            return lab_vm_bytecode_dissassemble_instruction_simple("nil value", index);
        }

        case LAB_VM_OP_EQUAL: {
            return lab_vm_bytecode_dissassemble_instruction_simple("equal", index);
        }

        case LAB_VM_OP_GREATER: {
            return lab_vm_bytecode_dissassemble_instruction_simple("greater than", index);
        }

        case LAB_VM_OP_LESSER: {
            return lab_vm_bytecode_dissassemble_instruction_simple("lesser than", index);
        }

        case LAB_VM_OP_NEGATE: {
            return lab_vm_bytecode_dissassemble_instruction_simple("negate", index);
        }

        case LAB_VM_OP_NOT: {
            return lab_vm_bytecode_dissassemble_instruction_simple("not", index);
        }

        case LAB_VM_OP_AND: {
            return lab_vm_bytecode_dissassemble_instruction_simple("and", index);
        }

        case LAB_VM_OP_ADD: {
            return lab_vm_bytecode_dissassemble_instruction_simple("add", index);
        }

        case LAB_VM_OP_SUBTRACT: {
            return lab_vm_bytecode_dissassemble_instruction_simple("subtract", index);
        }

        case LAB_VM_OP_MULTIPLY: {
            return lab_vm_bytecode_dissassemble_instruction_simple("multiply", index);
        }
    
        case LAB_VM_OP_DIVIDE: {
            return lab_vm_bytecode_dissassemble_instruction_simple("divide", index);
        }

        case LAB_VM_OP_RETURN: {
            return lab_vm_bytecode_dissassemble_instruction_simple("return", index);
        }

        default: {
            lab_errorln("Unkown opcode \'%d\'", instruction);
            return index + 1;
        }
    }

}

/*
    Serial Format

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
uint8_t* lab_vm_bytecode_serialize  (lab_vm_bytecode_t* bytecode, size_t* size) {

    #define HEADER_OFFSET (sizeof(uint32_t) * 6)

    uint32_t header[6] = {0, 0, 0, 0, 0, 0};

    #define constants_start_index header[0]
    #define constants_len header[1]
    #define bytecode_start_index header[2]
    #define bytecode_len header[3]
    #define line_data_start_index header[4]
    #define line_data_len header[5]

    /* Calculates each length */
    for(size_t i = 0; i < lab_vec_size(&bytecode->constants); i++) {
        for(size_t j = 0; j < sizeof(lab_vm_value_t); j++) {
            constants_len++;
        }
    }
    for(size_t i = 0; i < lab_vec_size(&bytecode->bytes); i++) {
        for(size_t j = 0; j < sizeof(uint8_t); j++) {
            bytecode_len++;
        }
    }
    for(size_t i = 0; i < lab_vec_size(&bytecode->lines); i++) {
        for(size_t j = 0; j < sizeof(int); j++) {
            line_data_len++;
        }
    }

    /* Calulate header */
    constants_start_index = HEADER_OFFSET;
    bytecode_start_index  = HEADER_OFFSET + constants_len;
    line_data_start_index = HEADER_OFFSET + constants_len + bytecode_len;
    uint32_t total_len    = HEADER_OFFSET + constants_len + bytecode_len + line_data_len;

    lab_vec_t serialized;
    lab_vec_init(&serialized, 1, total_len);

    lab_vec_push_back_arr(&serialized, header, sizeof(header));
    lab_vec_push_back_arr(&serialized, bytecode->constants.raw_data, bytecode->constants.used_size * bytecode->constants.type_size);
    lab_vec_push_back_arr(&serialized, bytecode->bytes.raw_data, bytecode->bytes.used_size         * bytecode->bytes.type_size);
    lab_vec_push_back_arr(&serialized, bytecode->lines.raw_data, bytecode->lines.used_size         * bytecode->lines.type_size);

    *size = serialized.used_size;

    return serialized.raw_data;

    #undef HEADER_OFFSET
}

bool     lab_vm_bytecode_deserialize(lab_vm_bytecode_t* bytecode, uint8_t* data) {
    lab_vm_bytecode_free(bytecode);

    uint32_t* header = (uint32_t*)data;

    #define constants_start_index header[0]
    #define constants_len header[1]
    #define bytecode_start_index header[2]
    #define bytecode_len header[3]
    #define line_data_start_index header[4]
    #define line_data_len header[5]

    lab_vec_init(&bytecode->constants, sizeof(lab_vm_value_t),   constants_len / sizeof(lab_vm_value_t));
    lab_vec_init(&bytecode->bytes,     sizeof(uint8_t),          bytecode_len);
    lab_vec_init(&bytecode->lines,     sizeof(int),              line_data_len / sizeof(int));

    lab_vec_push_back_arr(&bytecode->constants, &data[constants_start_index], constants_len / sizeof(lab_vm_value_t));
    lab_vec_push_back_arr(&bytecode->bytes,     &data[bytecode_start_index],  bytecode_len);
    lab_vec_push_back_arr(&bytecode->lines,     &data[line_data_len],         line_data_len / sizeof(int));

    return true;

}

bool lab_vm_init(lab_vm_t* vm) {
    if(!lab_vec_init(&vm->stack, sizeof(lab_vm_value_t), 256)) {
        return false;
    }
    lab_vm_reset_stack(vm);
    return true;
}

void lab_vm_free(lab_vm_t* vm) {
    lab_vec_free(&vm->stack);
}

void lab_vm_reset_stack(lab_vm_t* vm) {
    vm->stack_top = (lab_vm_value_t*)lab_vec_at_raw_alloc(&vm->stack, 0);    
}

void lab_vm_push(lab_vm_t* vm, lab_vm_value_t value) {
    *vm->stack_top = value;
    vm->stack_top++;
}

lab_vm_value_t lab_vm_pop(lab_vm_t* vm) {
    vm->stack_top--;
    return *vm->stack_top;
}

lab_vm_value_t lab_vm_peek(lab_vm_t* vm, int distance) {
    return vm->stack_top[-1 - distance];
}

static void runtime_error(lab_vm_t* vm, const char* fmt, ...) {
    lab_error("[Instruction: %d]: ", *(int*)lab_vec_at(&vm->bytecode->lines, vm->ip - (uint8_t*)lab_vec_at(&vm->bytecode->bytes, 0)));

    va_list args;
    va_start(args, fmt);
    vfprintf(lab_error_stream(), fmt, args);
    va_end(args);

    lab_vm_reset_stack(vm);
}

lab_vm_interpret_result_e_t lab_vm_interpret_bytecode(lab_vm_t* vm, lab_vm_bytecode_t* bytecode, bool debug_trace) {
    vm->bytecode = bytecode;
    vm->ip = (uint8_t*)lab_vec_at(&vm->bytecode->bytes, 0);
    return lab_vm_run(vm, debug_trace);
}

lab_vm_interpret_result_e_t lab_vm_run(lab_vm_t* vm, bool debug_trace) {

#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (*(lab_vm_value_t*)lab_vec_at(&vm->bytecode->constants, READ_BYTE()))
#define BINARY_OP(type, op) \
    do { \
        if(!LAB_VM_VALUE_IS_NUMBER(lab_vm_peek(vm, 0)) || !LAB_VM_VALUE_IS_NUMBER(lab_vm_peek(vm, 1))) { \
            runtime_error(vm, "Binary operands must be numbers\n"); \
            return LAB_VM_INTERPRET_RESULT_RUNTIME_ERROR; \
        } \
        double b = lab_vm_pop(vm).as.number; \
        double a = lab_vm_pop(vm).as.number; \
        lab_vm_push(vm, type(a op b)); \
    } while (false)

    for(;;) {

        if(debug_trace) {
            lab_print("[stack]:  ");
            for(lab_vm_value_t* slot = (lab_vm_value_t*)lab_vec_at_raw_alloc(&vm->stack, 0); slot < vm->stack_top; slot++) {
                lab_print_raw(LAB_ANSI_COLOR_YELLOW"[ ");
                lab_vm_value_print(*slot);
                lab_print_raw(" ]"LAB_ANSI_COLOR_RESET);
            }
            lab_print_raw("\n");
            lab_vm_bytecode_dissassemble_instruction(vm->bytecode, (size_t)(vm->ip - (uint8_t*)lab_vec_at(&vm->bytecode->bytes, 0)));
        }

        uint8_t instruction;
        switch(instruction = READ_BYTE()) {

            case LAB_VM_OP_CONSTANT: {
                lab_vm_value_t value = READ_CONSTANT();
                lab_vm_push(vm, value);
            }
            break;

            case LAB_VM_OP_CONSTANT_2L: {
                lab_vm_value_t value = (*(lab_vm_value_t*)lab_vec_at(&vm->bytecode->constants, *(short*)(vm->ip++)));
                ++vm->ip;
                lab_vm_push(vm, value);
            }
            break;

            case LAB_VM_OP_TRUE: {
                lab_vm_push(vm, LAB_VM_VALUE_BOOL(true));
            }
            break;

            case LAB_VM_OP_FALSE: {
                lab_vm_push(vm, LAB_VM_VALUE_BOOL(false));
            }
            break;

            case LAB_VM_OP_EQUAL: {
                lab_vm_value_t a = lab_vm_pop(vm);
                lab_vm_value_t b = lab_vm_pop(vm);
                lab_vm_push(vm, LAB_VM_VALUE_BOOL(lab_vm_value_is_equal(a, b)));
            }
            break;

            case LAB_VM_OP_GREATER: {
                BINARY_OP(LAB_VM_VALUE_BOOL, >);
            }
            break;

            case LAB_VM_OP_LESSER: {
                BINARY_OP(LAB_VM_VALUE_BOOL, <);
            }
            break;

            case LAB_VM_OP_NIL: {
                lab_vm_push(vm, LAB_VM_VALUE_NIL);
            }
            break;

            case LAB_VM_OP_NEGATE: {
                if(!LAB_VM_VALUE_IS_NUMBER(lab_vm_peek(vm, 0))) {
                    runtime_error(vm, "Operand must be a number\n");
                    return LAB_VM_INTERPRET_RESULT_RUNTIME_ERROR;
                }
                lab_vm_push(vm, LAB_VM_VALUE_NUMBER(-(lab_vm_pop(vm).as.number)));
            }
            break;

            case LAB_VM_OP_NOT: {
                lab_vm_push(vm, LAB_VM_VALUE_BOOL(lab_vm_value_is_falsey(lab_vm_pop(vm))));
            }
            break;

            case LAB_VM_OP_AND: {
                 if(!LAB_VM_VALUE_IS_BOOL(lab_vm_peek(vm, 0)) || !LAB_VM_VALUE_IS_BOOL(lab_vm_peek(vm, 1))) {
                     runtime_error(vm, "Two operands must be booleans!\n");
                     return LAB_VM_INTERPRET_RESULT_RUNTIME_ERROR;
                 } else {
                    bool b = lab_vm_pop(vm).as.boolean;
                    bool a = lab_vm_pop(vm).as.boolean;
                    lab_vm_push(vm, LAB_VM_VALUE_BOOL(a && b));
                 }
            }
            break;

            case LAB_VM_OP_ADD: {
                BINARY_OP(LAB_VM_VALUE_NUMBER, +);
            }
            break;

            case LAB_VM_OP_SUBTRACT: {
                BINARY_OP(LAB_VM_VALUE_NUMBER, -);
            }
            break;

            case LAB_VM_OP_MULTIPLY: {
                BINARY_OP(LAB_VM_VALUE_NUMBER, *);
            }
            break;
        
            case LAB_VM_OP_DIVIDE: {
                BINARY_OP(LAB_VM_VALUE_NUMBER, /);
            }
            break;

            case LAB_VM_OP_RETURN: {
                lab_success("[return]: "LAB_ANSI_COLOR_YELLOW);
                lab_vm_value_print(lab_vm_pop(vm));
                lab_print_raw("\n"LAB_ANSI_COLOR_RESET);
                return LAB_VM_INTERPRET_RESULT_SUCCESS;
            }
            break;

        }
    }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE

}