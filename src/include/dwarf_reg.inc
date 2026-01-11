#ifndef __DWARF_REG_INC__
#define __DWARF_REG_INC__

#include "platform.inc"

// DWARF register numbers for exception handling
#ifdef ARCH_X86_64
    #define DW_REG_RA       16      // Return address (RIP)
    #define DW_REG_EXC      15      // r15 - callee-saved for exception object
    #define DW_REG_EXC_ASM  %r15
#else // ARCH_ARM64
    #define DW_REG_RA       30      // Return address (LR/x30)
    #define DW_REG_EXC      28      // x28 - callee-saved for exception object
    #define DW_REG_EXC_ASM  x28
#endif

#endif
