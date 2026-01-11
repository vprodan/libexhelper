#ifndef __DWARF_INC__
#define __DWARF_INC__

#include "dwarf_reg.inc"

// DW_EH_PE Encodings (Pointer encodings for LSDA & personalities)
#define DW_EH_PE_absptr          0x00
#define DW_EH_PE_uleb128         0x01
#define DW_EH_PE_udata2          0x02
#define DW_EH_PE_udata4          0x03
#define DW_EH_PE_udata8          0x04
#define DW_EH_PE_signed          0x08
#define DW_EH_PE_sleb128         0x09
#define DW_EH_PE_sdata2          0x0A
#define DW_EH_PE_sdata4          0x0B
#define DW_EH_PE_sdata8          0x0C

#define DW_EH_PE_pcrel           0x10
#define DW_EH_PE_textrel         0x20
#define DW_EH_PE_datarel         0x30
#define DW_EH_PE_funcrel         0x40
#define DW_EH_PE_aligned         0x50

#define DW_EH_PE_indirect        0x80
#define DW_EH_PE_omit            0xFF

#define LSDA_ENC DW_EH_PE_pcrel
#ifdef OS_MACOS
    #define PERSONALITY_ENC DW_EH_PE_pcrel | DW_EH_PE_sdata4 | DW_EH_PE_indirect
#else
    #define PERSONALITY_ENC DW_EH_PE_pcrel | DW_EH_PE_sdata4
#endif

#endif
