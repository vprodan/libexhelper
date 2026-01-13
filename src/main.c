#include <stdint.h>
#include <unwind.h>
#include "dwarf_reg.inc"

#define EXPORT __attribute__((visibility("default")))
#define HIDDEN __attribute__((visibility("hidden")))

// Thread-local variable that stores the current exception pointer
HIDDEN __thread void *current_exception_ptr = 0;

// Public API: Returns a pointer to the current exception pointer
EXPORT void **eh_get_exception_ptr(void)
{
    return &current_exception_ptr;
}

// Personality function for exception handling
HIDDEN _Unwind_Reason_Code personality(
    int version,
    _Unwind_Action actions,
    _Unwind_Exception_Class exceptionClass,
    struct _Unwind_Exception *exceptionObject,
    struct _Unwind_Context *context)
{
    // Check if this is a forced unwind
    if (actions & _UA_FORCE_UNWIND)
    {
        return _URC_CONTINUE_UNWIND;
    }

    // Get language-specific data area (LSDA)
    void *lsdaAddr = (void *)_Unwind_GetLanguageSpecificData(context);
    if (!lsdaAddr)
    {
        return _URC_CONTINUE_UNWIND;
    }

    int lsdaValue = *(int *)lsdaAddr;

    if (actions & _UA_SEARCH_PHASE)
    {
        // Search phase - check if we have a handler
        if (lsdaValue == 0)
        {
            return _URC_CONTINUE_UNWIND;
        }
        return _URC_HANDLER_FOUND;
    }
    else
    {
        // Handler phase - set up for handler execution
        if (actions & _UA_HANDLER_FRAME)
        {
            // Calculate landing pad address
            uintptr_t landingPad = (uintptr_t)lsdaAddr + lsdaValue;
            _Unwind_SetIP(context, landingPad);
            _Unwind_SetGR(context, DW_REG_EXC, (uintptr_t)exceptionObject);
            return _URC_INSTALL_CONTEXT;
        }
        return _URC_CONTINUE_UNWIND;
    }
}
