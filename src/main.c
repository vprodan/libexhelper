#include <stdint.h>
#include <unwind.h>
#include "dwarf_reg.inc"

// This is a thread-local variable that stores the current exception pointer
// for the current thread.
__thread void *current_exception_ptr = 0;

// This function returns a pointer to the current exception pointer.
void **get_exception_ptr(void)
{
    return &current_exception_ptr;
}

// This function is the personality function for exception handling.
_Unwind_Reason_Code personality(
    int version,
    _Unwind_Action actions,
    _Unwind_Exception_Class exceptionClass,
    struct _Unwind_Exception *exceptionObject,
    struct _Unwind_Context *context)
{
    // Check if this is a forced unwind
    if (actions & _UA_FORCE_UNWIND)
    {
        // Just continue unwinding if forced
        return _URC_CONTINUE_UNWIND;
    }

    // Get language-specific data area (LSDA)
    void *lsdaAddr = (void *)_Unwind_GetLanguageSpecificData(context);
    int lsdaValue = *(int *)lsdaAddr;

    if (actions & _UA_SEARCH_PHASE)
    {
        // This is the search phase - determine if we can handle this exception

        // Check if we have a handler by testing if LSDA value is non-zero
        if (lsdaValue == 0)
        {
            // No handler available
            return _URC_CONTINUE_UNWIND;
        }

        // We have a handler
        return _URC_HANDLER_FOUND;
    }
    else
    {
        // This is the handler phase - set up for handler execution

        // Make sure we're actually supposed to handle this frame
        if (actions & _UA_HANDLER_FRAME)
        {
            // Calculate landing pad address by adding LSDA value to LSDA address
            uintptr_t landingPad = (uintptr_t)lsdaAddr + lsdaValue;

            // Set instruction pointer to landing pad
            _Unwind_SetIP(context, landingPad);

            // Save exception object in r15 register
            _Unwind_SetGR(context, DW_REG_EXC, (uintptr_t)exceptionObject);

            // Tell unwinder to resume execution at the landing pad
            return _URC_INSTALL_CONTEXT;
        }

        return _URC_CONTINUE_UNWIND;
    }
}
