#ifndef __COMMON_INC__
#define __COMMON_INC__

#include "dwarf.inc"

#if defined(__APPLE__)
    #define SYMBOL(name) _##name
    #define LSDA_SECTION .section __TEXT,__gcc_except_tab
    // macOS assembler requires $$ in macros
    #define IMM(val) $$val
    #define GNU_STACK_NOTE
#else
    #define SYMBOL(name) name
    #define LSDA_SECTION .section .gcc_except_table,"a",@progbits
    // Linux assembler requires single $
    #define IMM(val) $val
    // Mark stack as non-executable (security)
    #define GNU_STACK_NOTE .section .note.GNU-stack,"",@progbits
#endif

#if defined(__x86_64__)
    // Register used to hold the target address for exception handling
    #define REG_EH_TARGET       %rax
    // Register used to hold return values
    #define REG_RET             %rax
    // Register used to hold rethrown exception pointer
    #define REG_RETHROW_EXC     %r11
#elif defined(__aarch64__) || defined(__arm64__)
    // Register used to hold the target address for exception handling
    #define REG_EH_TARGET       x9
    // Register used to hold return values
    #define REG_RET             x0
    // Register used to hold rethrown exception pointer
    #define REG_RETHROW_EXC     x9
#else
    #error "Unsupported architecture"
#endif

// Function prolog
.macro FRAME_PROLOG
#if defined(__x86_64__)
    push    %rbp
    .cfi_def_cfa_offset 16
    .cfi_offset %rbp, -16
    mov     %rsp, %rbp
    .cfi_def_cfa_register %rbp
#elif defined(__aarch64__) || defined(__arm64__)
    stp     x29, x30, [sp, #-16]!
    .cfi_def_cfa_offset 16
    .cfi_offset x29, -16
    .cfi_offset x30, -8
    mov     x29, sp
    .cfi_def_cfa_register x29
#else
    #error "Unsupported architecture"
#endif
.endm

// Function epilog
.macro FRAME_EPILOG
#if defined(__x86_64__)
    mov     %rbp, %rsp
    .cfi_def_cfa_register %rsp
    pop     %rbp
    .cfi_def_cfa_offset 8
    .cfi_restore %rbp
#elif defined(__aarch64__) || defined(__arm64__)
    mov     sp, x29
    .cfi_def_cfa_register sp
    ldp     x29, x30, [sp], #16
    .cfi_def_cfa_offset 0
    .cfi_restore x29
    .cfi_restore x30
#else
    #error "Unsupported architecture"
#endif
.endm

// Call a label
.macro CALL_LABEL label:req
#if defined(__x86_64__)
    call    \label
#elif defined(__aarch64__) || defined(__arm64__)
    bl      \label
#else
    #error "Unsupported architecture"
#endif
.endm

.macro JUMP_LABEL label:req
#if defined(__x86_64__)
    jmp     \label
#elif defined(__aarch64__) || defined(__arm64__)
    b       \label
#else
    #error "Unsupported architecture"
#endif
.endm

// Call a register
.macro CALL_REG reg:req
#if defined(__x86_64__)
    call    *\reg
#elif defined(__aarch64__) || defined(__arm64__)
    blr     \reg
#else
    #error "Unsupported architecture"
#endif
.endm

// Clear a register
.macro CLEAR_REG reg:req
#if defined(__x86_64__)
    xor     \reg, \reg
#elif defined(__aarch64__) || defined(__arm64__)
    mov     \reg, xzr
#else
    #error "Unsupported architecture"
#endif
.endm

// Set first argument from register
.macro ARG_SET_FIRST reg:req
#if defined(__x86_64__)
    mov    \reg, %rdi
#elif defined(__aarch64__) || defined(__arm64__)
    mov     x0, \reg
#else
    #error "Unsupported architecture"
#endif
.endm

// Jump to label if register is not zero
.macro JUMP_IF_NZ reg:req, label:req
#if defined(__x86_64__)
    test    \reg, \reg
    jnz     \label
#elif defined(__aarch64__) || defined(__arm64__)
    cbnz    \reg, \label
#else
    #error "Unsupported architecture"
#endif
.endm

// Exit with code
.macro EXIT_WITH_CODE code:req
#if defined(__x86_64__)
    mov $\code, %rdi
    CALL_LABEL SYMBOL(exit)
    ud2
#elif defined(__aarch64__) || defined(__arm64__)
    mov     x0, #\code
    CALL_LABEL SYMBOL(exit)
    brk     #1
#else
    #error "Unsupported architecture"
#endif
.endm

// Save argument registers + REG_EH_TARGET register
.macro ARGS_SAVE
#if defined(__x86_64__)
    sub     IMM(72), %rsp
    .cfi_adjust_cfa_offset 72
    
    mov     %rcx, 0(%rsp)
    .cfi_rel_offset %rcx, 0
    mov     %rdx, 8(%rsp)
    .cfi_rel_offset %rdx, 8
    mov     %rsi, 16(%rsp)
    .cfi_rel_offset %rsi, 16
    mov     %rdi, 24(%rsp)
    .cfi_rel_offset %rdi, 24
    mov     %r8, 32(%rsp)
    .cfi_rel_offset %r8, 32
    mov     %r9, 40(%rsp)
    .cfi_rel_offset %r9, 40
    mov     %r10, 48(%rsp)
    .cfi_rel_offset %r10, 48
    mov     %r11, 56(%rsp)
    .cfi_rel_offset %r11, 56
    mov     REG_EH_TARGET, 64(%rsp)
    .cfi_rel_offset REG_EH_TARGET, 64
#elif defined(__aarch64__) || defined(__arm64__)
    sub     sp, sp, #80
    .cfi_adjust_cfa_offset 80

    str     x0, [sp, #0]
    .cfi_rel_offset x0, 0
    str     x1, [sp, #8]
    .cfi_rel_offset x1, 8
    str     x2, [sp, #16]
    .cfi_rel_offset x2, 16
    str     x3, [sp, #24]
    .cfi_rel_offset x3, 24
    str     x4, [sp, #32]
    .cfi_rel_offset x4, 32
    str     x5, [sp, #40]
    .cfi_rel_offset x5, 40
    str     x6, [sp, #48]
    .cfi_rel_offset x6, 48
    str     x7, [sp, #56]
    .cfi_rel_offset x7, 56
    str     REG_EH_TARGET, [sp, #64]
    .cfi_rel_offset REG_EH_TARGET, 64
#else
    #error "Unsupported architecture"
#endif
.endm

// Restore argument registers + REG_EH_TARGET register
.macro ARGS_RESTORE
#if defined(__x86_64__)
    mov    0(%rsp), %rcx
    .cfi_restore %rcx
    mov    8(%rsp), %rdx
    .cfi_restore %rdx
    mov    16(%rsp), %rsi
    .cfi_restore %rsi
    mov    24(%rsp), %rdi
    .cfi_restore %rdi
    mov    32(%rsp), %r8
    .cfi_restore %r8
    mov    40(%rsp), %r9
    .cfi_restore %r9
    mov    48(%rsp), %r10
    .cfi_restore %r10
    mov    56(%rsp), %r11
    .cfi_restore %r11
    mov    64(%rsp), REG_EH_TARGET
    .cfi_restore REG_EH_TARGET

    add    IMM(72), %rsp
    .cfi_adjust_cfa_offset -72
#elif defined(__aarch64__) || defined(__arm64__)
    ldr     x0, [sp, #0]
    .cfi_restore x0
    ldr     x1, [sp, #8]
    .cfi_restore x1
    ldr     x2, [sp, #16]
    .cfi_restore x2
    ldr     x3, [sp, #24]
    .cfi_restore x3
    ldr     x4, [sp, #32]
    .cfi_restore x4
    ldr     x5, [sp, #40]
    .cfi_restore x5
    ldr     x6, [sp, #48]
    .cfi_restore x6
    ldr     x7, [sp, #56]
    .cfi_restore x7
    ldr     REG_EH_TARGET, [sp, #64]
    .cfi_restore REG_EH_TARGET

    add     sp, sp, #80
    .cfi_adjust_cfa_offset -80
#else
    #error "Unsupported architecture"
#endif
.endm

// Set exception object pointer
.macro EXC_SET source:req
#if defined(__x86_64__)
    mov     DW_REG_EXC_ASM, (\source)
#elif defined(__aarch64__) || defined(__arm64__)
    str     DW_REG_EXC_ASM, [\source]
#else
    #error "Unsupported architecture"
#endif
.endm

// Clear exception pointer
.macro EXC_CLEAR
    ARGS_SAVE
    CALL_LABEL SYMBOL(get_exception_ptr)
#if defined(__x86_64__)
    xor     %r10, %r10               // null pointer
    mov     %r10, (REG_RET)          // clear exception pointer
#elif defined(__aarch64__) || defined(__arm64__)
    str     xzr, [REG_RET]               // clear exception pointer
#else
    #error "Unsupported architecture"
#endif
    ARGS_RESTORE
.endm

// Get exception pointer into destination register
.macro EXC_GET dest:req
    CALL_LABEL SYMBOL(get_exception_ptr)
#if defined(__x86_64__)
    mov     (REG_RET), \dest
#elif defined(__aarch64__) || defined(__arm64__)
    ldr     \dest, [REG_RET]
#else
    #error "Unsupported architecture"
#endif
.endm

// Save exception object pointer to stack
.macro EXC_SAVE
#if defined(__x86_64__)
    sub     IMM(16), %rsp
    .cfi_adjust_cfa_offset 16
    mov     DW_REG_EXC_ASM, 0(%rsp)
#elif defined(__aarch64__) || defined(__arm64__)
    str     DW_REG_EXC_ASM, [sp, #-16]!
    .cfi_adjust_cfa_offset 16
#else
    #error "Unsupported architecture"
#endif
    .cfi_rel_offset DW_REG_EXC_ASM, 0
.endm

// Restore exception object pointer from stack
.macro EXC_RESTORE
#if defined(__x86_64__)
    mov     (%rsp), DW_REG_EXC_ASM
    add     IMM(16), %rsp
    .cfi_adjust_cfa_offset -16
#elif defined(__aarch64__) || defined(__arm64__)
    ldr     DW_REG_EXC_ASM, [sp], #16
    .cfi_adjust_cfa_offset -16
#else
    #error "Unsupported architecture"
#endif
    .cfi_restore DW_REG_EXC_ASM
.endm

// Wrap target with exception handler
.macro WRAP_EH eh:req, target:req
#if defined(__x86_64__)
    lea     \target(%rip), REG_EH_TARGET
    jmp     \eh
#elif defined(__aarch64__) || defined(__arm64__)
    #if defined(__APPLE__)
        adrp    REG_EH_TARGET, \target@PAGE
        add     REG_EH_TARGET, REG_EH_TARGET, \target@PAGEOFF
    #else
        adrp    REG_EH_TARGET, \target
        add     REG_EH_TARGET, REG_EH_TARGET, :lo12:\target
    #endif
    b       \eh
#else
    #error "Unsupported architecture"
#endif
.endm

#endif // __COMMON_INC__