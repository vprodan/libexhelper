#ifndef __COMMON_INC__
#define __COMMON_INC__

#include "platform.inc"
#include "dwarf.inc"

//==============================================================================
// Architecture-specific Register Definitions
//==============================================================================

#ifdef ARCH_X86_64
    #define REG_EH_TARGET       %rax
    #define REG_RET             %rax
    #define REG_RETHROW_EXC     %r11
#else // ARCH_ARM64
    #define REG_EH_TARGET       x9
    #define REG_RET             x0
    #define REG_RETHROW_EXC     x9
#endif

//==============================================================================
// Macros
//==============================================================================

.macro FRAME_PROLOG
#ifdef ARCH_X86_64
    push    %rbp
    .cfi_def_cfa_offset 16
    .cfi_offset %rbp, -16
    mov     %rsp, %rbp
    .cfi_def_cfa_register %rbp
#else
    stp     x29, x30, [sp, #-16]!
    .cfi_def_cfa_offset 16
    .cfi_offset x29, -16
    .cfi_offset x30, -8
    mov     x29, sp
    .cfi_def_cfa_register x29
#endif
.endm

.macro FRAME_EPILOG
#ifdef ARCH_X86_64
    mov     %rbp, %rsp
    .cfi_def_cfa_register %rsp
    pop     %rbp
    .cfi_def_cfa_offset 8
    .cfi_restore %rbp
#else
    mov     sp, x29
    .cfi_def_cfa_register sp
    ldp     x29, x30, [sp], #16
    .cfi_def_cfa_offset 0
    .cfi_restore x29
    .cfi_restore x30
#endif
.endm

.macro CALL_LABEL label:req
#ifdef ARCH_X86_64
    call    \label
#else
    bl      \label
#endif
.endm

.macro JUMP_LABEL label:req
#ifdef ARCH_X86_64
    jmp     \label
#else
    b       \label
#endif
.endm

.macro CALL_REG reg:req
#ifdef ARCH_X86_64
    call    *\reg
#else
    blr     \reg
#endif
.endm

.macro CLEAR_REG reg:req
#ifdef ARCH_X86_64
    xor     \reg, \reg
#else
    mov     \reg, xzr
#endif
.endm

.macro ARG_SET_FIRST reg:req
#ifdef ARCH_X86_64
    mov    \reg, %rdi
#else
    mov     x0, \reg
#endif
.endm

.macro JUMP_IF_NZ reg:req, label:req
#ifdef ARCH_X86_64
    test    \reg, \reg
    jnz     \label
#else
    cbnz    \reg, \label
#endif
.endm

.macro EXIT_WITH_CODE code:req
#ifdef ARCH_X86_64
    mov $\code, %rdi
    CALL_LABEL SYMBOL(exit)
    ud2
#else
    mov     x0, #\code
    CALL_LABEL SYMBOL(exit)
    brk     #1
#endif
.endm

.macro ARGS_SAVE
#ifdef ARCH_X86_64
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
#else
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
#endif
.endm

.macro ARGS_RESTORE
#ifdef ARCH_X86_64
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
#else
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
#endif
.endm

.macro EXC_SET source:req
#ifdef ARCH_X86_64
    mov     DW_REG_EXC_ASM, (\source)
#else
    str     DW_REG_EXC_ASM, [\source]
#endif
.endm

.macro EXC_CLEAR
    ARGS_SAVE
    CALL_LABEL SYMBOL(get_exception_ptr)
#ifdef ARCH_X86_64
    xor     %r10, %r10
    mov     %r10, (REG_RET)
#else
    str     xzr, [REG_RET]
#endif
    ARGS_RESTORE
.endm

.macro EXC_GET dest:req
    CALL_LABEL SYMBOL(get_exception_ptr)
#ifdef ARCH_X86_64
    mov     (REG_RET), \dest
#else
    ldr     \dest, [REG_RET]
#endif
.endm

.macro EXC_SAVE
#ifdef ARCH_X86_64
    sub     IMM(16), %rsp
    .cfi_adjust_cfa_offset 16
    mov     DW_REG_EXC_ASM, 0(%rsp)
#else
    str     DW_REG_EXC_ASM, [sp, #-16]!
    .cfi_adjust_cfa_offset 16
#endif
    .cfi_rel_offset DW_REG_EXC_ASM, 0
.endm

.macro EXC_RESTORE
#ifdef ARCH_X86_64
    mov     (%rsp), DW_REG_EXC_ASM
    add     IMM(16), %rsp
    .cfi_adjust_cfa_offset -16
#else
    ldr     DW_REG_EXC_ASM, [sp], #16
    .cfi_adjust_cfa_offset -16
#endif
    .cfi_restore DW_REG_EXC_ASM
.endm

.macro WRAP_EH eh:req, target:req
#ifdef ARCH_X86_64
    lea     \target(%rip), REG_EH_TARGET
    jmp     \eh
#else
    #ifdef OS_MACOS
        adrp    REG_EH_TARGET, \target@PAGE
        add     REG_EH_TARGET, REG_EH_TARGET, \target@PAGEOFF
    #else
        adrp    REG_EH_TARGET, \target
        add     REG_EH_TARGET, REG_EH_TARGET, :lo12:\target
    #endif
    b       \eh
#endif
.endm

#endif // __COMMON_INC__