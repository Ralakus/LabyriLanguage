#include "vm.h"

void lab_vm_value_print(lab_vm_value_t value) {
    switch (value.type) {
        case LAB_VM_VALUE_TYPE_NIL: 
            lab_print_raw("nil");
        break;

        case LAB_VM_VALUE_TYPE_BOOLEAN:
            if(value.as.boolean == true) {
                lab_print_raw("true");
            } else {
                lab_print_raw("false");
            }
        break;

        case LAB_VM_VALUE_TYPE_NUMBER:
            lab_print_raw("%g", value.as.number);
        break;
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

        case LAB_VM_OP_NEGATE: {
            return lab_vm_bytecode_dissassemble_instruction_simple("negate", index);
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

lab_vm_interpret_result_e_t lab_vm_interpret_bytecode(lab_vm_t* vm, lab_vm_bytecode_t* bytecode, bool debug_trace) {
    vm->bytecode = bytecode;
    vm->ip = (uint8_t*)lab_vec_at(&vm->bytecode->bytes, 0);
    return lab_vm_run(vm, debug_trace);
}

lab_vm_interpret_result_e_t lab_vm_run(lab_vm_t* vm, bool debug_trace) {

#define READ_BYTE() (*vm->ip++)
#define READ_CONSTANT() (*(lab_vm_value_t*)lab_vec_at(&vm->bytecode->constants, READ_BYTE()))
#define BINARY_OP(op) \
    do { \
      double b = lab_vm_pop(vm).as.number; \
      double a = lab_vm_pop(vm).as.number; \
      lab_vm_push(vm, LAB_VM_VALUE_NUMBER(a op b)); \
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

            case LAB_VM_OP_NEGATE: {
                lab_vm_push(vm, LAB_VM_VALUE_NUMBER(-(lab_vm_pop(vm).as.number)));
            }
            break;

            case LAB_VM_OP_ADD: {
                BINARY_OP(+);
            }
            break;

            case LAB_VM_OP_SUBTRACT: {
                BINARY_OP(-);
            }
            break;

            case LAB_VM_OP_MULTIPLY: {
                BINARY_OP(*);
            }
            break;
        
            case LAB_VM_OP_DIVIDE: {
                BINARY_OP(/);
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