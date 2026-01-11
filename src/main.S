#include "common.inc"

.cfi_sections .eh_frame

.global SYMBOL(eh_managed_to_native)
.global SYMBOL(eh_native_to_managed)

LSDA_SECTION
.LSDA_none:
    .long 0
.LSDA_landingpad:
    .long .Llandingpad - .

.text
.Lpersonality:
    JUMP_LABEL SYMBOL(personality)

SYMBOL(eh_managed_to_native):
.cfi_startproc
.cfi_personality PERSONALITY_ENC, .Lpersonality
.cfi_lsda LSDA_ENC, .LSDA_landingpad
    FRAME_PROLOG
    EXC_SAVE
    CALL_REG REG_EH_TARGET
.cfi_remember_state
    EXC_RESTORE
    FRAME_EPILOG
    ret
.cfi_restore_state
.cfi_endproc

.Llandingpad:
.cfi_startproc
    ARGS_SAVE
    CALL_LABEL SYMBOL(get_exception_ptr)
    EXC_SET REG_RET
    CLEAR_REG REG_RET
    ARGS_RESTORE
    EXC_RESTORE
    FRAME_EPILOG
    ret
.cfi_endproc

SYMBOL(eh_native_to_managed):
.cfi_startproc
.cfi_personality PERSONALITY_ENC, .Lpersonality
.cfi_lsda LSDA_ENC, .LSDA_none
    FRAME_PROLOG
    EXC_CLEAR
    CALL_REG REG_EH_TARGET

    EXC_GET REG_RETHROW_EXC
    JUMP_IF_NZ REG_RETHROW_EXC, .Ldo_rethrow
.cfi_remember_state
    FRAME_EPILOG
    ret
.cfi_restore_state

.Ldo_rethrow:
    ARG_SET_FIRST REG_RETHROW_EXC
    CALL_LABEL SYMBOL(_Unwind_RaiseException)
    EXIT_WITH_CODE 69
.cfi_endproc

GNU_STACK_NOTE
