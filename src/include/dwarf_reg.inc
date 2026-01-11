#ifndef __DWARF_REG_INC__
#define __DWARF_REG_INC__

#if defined(__x86_64__)
// DWARF Register Numbers for x86_64 (System V ABI)
    #define DW_REG_rax    0
    #define DW_REG_rdx    1
    #define DW_REG_rcx    2
    #define DW_REG_rbx    3
    #define DW_REG_rsi    4
    #define DW_REG_rdi    5
    #define DW_REG_rbp    6
    #define DW_REG_rsp    7
    #define DW_REG_r8     8
    #define DW_REG_r9     9
    #define DW_REG_r10   10
    #define DW_REG_r11   11
    #define DW_REG_r12   12
    #define DW_REG_r13   13
    #define DW_REG_r14   14
    #define DW_REG_r15   15
    #define DW_REG_RA    16    // Return address register = RIP

    #define DW_REG_EXC      DW_REG_r15  // Register to store exception object pointer
    #define DW_REG_EXC_ASM  %r15        // Corresponding assembly register name
#elif defined(__aarch64__) || defined(__arm64__)
// DWARF Register Numbers for AArch64
    #define DW_REG_x0     0
    #define DW_REG_x1     1
    #define DW_REG_x2     2
    #define DW_REG_x3     3
    #define DW_REG_x4     4
    #define DW_REG_x5     5
    #define DW_REG_x6     6
    #define DW_REG_x7     7
    #define DW_REG_x8     8
    #define DW_REG_x9     9
    #define DW_REG_x10   10
    #define DW_REG_x11   11
    #define DW_REG_x12   12
    #define DW_REG_x13   13
    #define DW_REG_x14   14
    #define DW_REG_x15   15
    #define DW_REG_x16   16
    #define DW_REG_x17   17
    #define DW_REG_x18   18
    #define DW_REG_x19   19
    #define DW_REG_x20   20
    #define DW_REG_x21   21
    #define DW_REG_x22   22
    #define DW_REG_x23   23
    #define DW_REG_x24   24
    #define DW_REG_x25   25
    #define DW_REG_x26   26
    #define DW_REG_x27   27
    #define DW_REG_x28   28
    #define DW_REG_x29   29  // Frame pointer (FP)
    #define DW_REG_x30   30  // Link register (LR)
    #define DW_REG_sp    31  // Stack pointer
    #define DW_REG_RA    30  // Return address = LR (x30)

    #define DW_REG_EXC      DW_REG_x28  // Callee-saved register for exception object
    #define DW_REG_EXC_ASM  x28         // Corresponding assembly register name
#else
    #error "Unsupported architecture"
#endif

#endif
