#include "lexer.h"
#include "vm.h"

int main(int argc, const char* argv[]) {

    lab_lexer_token_container_t tokens;
    lab_lexer_token_container_init(&tokens);
    
    char code[] = "x: str = \"Hello world!\"; # Test variable\nprint(x + \"\\n\");";
    char code2[] = "return -(14.0 + 0.48)";

    lab_lexer_lex(&tokens, code2);

    lab_lexer_token_container_print(&tokens);

    lab_lexer_token_container_free(&tokens);

    lab_vm_t vm;
    lab_vm_init(&vm);

    lab_vm_bytecode_t bytecode;
    lab_vm_bytecode_init(&bytecode, 8);

    // OP_CONSTANT number 14.0
    size_t constant = lab_vm_bytecode_write_constant(&bytecode, LAB_VM_VALUE_NUMBER(14.0));
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_CONSTANT);
    lab_vm_bytecode_write_byte(&bytecode, 0, constant);

    // OP_CONSTANT number 0.48
    constant = lab_vm_bytecode_write_constant(&bytecode, LAB_VM_VALUE_NUMBER(0.48));
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_CONSTANT);
    lab_vm_bytecode_write_byte(&bytecode, 0, constant);

    // OP_ADD
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_ADD);

    // OP_NEGATE
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_NEGATE);

    // OP_RETURN
    lab_vm_bytecode_write_byte(&bytecode, 0, LAB_VM_OP_RETURN);

    lab_vm_bytecode_dissassemble(&bytecode, "Test");

    lab_noticeln("--== Run Test ==--");

    if(lab_vm_interpret_bytecode(&vm, &bytecode, true) != LAB_VM_INTERPRET_RESULT_SUCCESS) {
        lab_errorln("Failed to execute bytecode!");
    }

    lab_vm_free(&vm);

    lab_vm_bytecode_free(&bytecode);

    return 0;

}