#ifndef __PLATFORM_INC__
#define __PLATFORM_INC__

//==============================================================================
// Platform and Architecture Detection
//==============================================================================

// Validate architecture
#if defined(__x86_64__)
    #define ARCH_X86_64 1
#elif defined(__aarch64__) || defined(__arm64__)
    #define ARCH_ARM64 1
#else
    #error "Unsupported architecture: only x86_64 and arm64 are supported"
#endif

// Validate OS
#if defined(__APPLE__)
    #define OS_MACOS 1
#elif defined(__linux__)
    #define OS_LINUX 1
#else
    #error "Unsupported OS: only macOS and Linux are supported"
#endif

//==============================================================================
// Platform-specific Assembly Definitions
//==============================================================================

#ifdef OS_MACOS
    #define SYMBOL(name) _##name
    #define LSDA_SECTION .section __TEXT,__gcc_except_tab
    #define IMM(val) $$val
    #define GNU_STACK_NOTE
#else
    #define SYMBOL(name) name
    #define LSDA_SECTION .section .gcc_except_table,"a",@progbits
    #define IMM(val) $val
    #define GNU_STACK_NOTE .section .note.GNU-stack,"",@progbits
#endif

#endif // __PLATFORM_INC__
