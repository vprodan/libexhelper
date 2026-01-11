#include "common.inc"
.text

// lib imports
.extern SYMBOL(eh_native_to_managed)
.extern SYMBOL(eh_managed_to_native)

// test.cpp imports
.extern SYMBOL(caller)
.extern SYMBOL(throw_ex)

.global SYMBOL(wrap_caller)
.global SYMBOL(wrap_throw_ex)

SYMBOL(wrap_caller):
    WRAP_EH SYMBOL(eh_native_to_managed), SYMBOL(caller)
SYMBOL(wrap_throw_ex):
    WRAP_EH SYMBOL(eh_managed_to_native), SYMBOL(throw_ex)
GNU_STACK_NOTE
