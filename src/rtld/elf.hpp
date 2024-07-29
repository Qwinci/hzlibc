#pragma once
#include "elf.h"

#if UINTPTR_MAX == UINT64_MAX

using Elf_Ehdr = Elf64_Ehdr;
using Elf_Phdr = Elf64_Phdr;
using Elf_Sym = Elf64_Sym;
using Elf_Rela = Elf64_Rela;
using Elf_Relr = Elf64_Relr;
using Elf_Rel = Elf64_Rel;
using Elf_Dyn = Elf64_Dyn;
using Elf_Aux = Elf64_auxv_t;
using Elf_Bloom = uint64_t;

static constexpr uintptr_t ELF_CLASS_BITS = 64;
static constexpr uint8_t ELF_CLASS = ELFCLASS64;

#define ELF_ST_BIND ELF64_ST_BIND
#define ELF_ST_TYPE ELF64_ST_TYPE
#define ELF_R_SYM ELF64_R_SYM
#define ELF_R_TYPE ELF64_R_TYPE

#else

using Elf_Ehdr = Elf32_Ehdr;
using Elf_Phdr = Elf32_Phdr;
using Elf_Sym = Elf32_Sym;
using Elf_Rela = Elf32_Rela;
using Elf_Relr = Elf32_Relr;
using Elf_Rel = Elf32_Rel;
using Elf_Dyn = Elf32_Dyn;
using Elf_Aux = Elf32_auxv_t;
using Elf_Bloom = uint32_t;

static constexpr uintptr_t ELF_CLASS_BITS = 32;
static constexpr uint8_t ELF_CLASS = ELFCLASS32;

#define ELF_ST_BIND ELF32_ST_BIND
#define ELF_ST_TYPE ELF32_ST_TYPE
#define ELF_R_SYM ELF32_R_SYM
#define ELF_R_TYPE ELF32_R_TYPE

#endif

#ifdef __x86_64__

static constexpr uint16_t ELF_MACHINE = EM_X86_64;

#define R_ABSOLUTE R_X86_64_64
#define R_COPY R_X86_64_COPY
#define R_GLOB_DAT R_X86_64_GLOB_DAT
#define R_JUMP_SLOT R_X86_64_JUMP_SLOT
#define R_RELATIVE R_X86_64_RELATIVE
#define R_DTPMOD R_X86_64_DTPMOD64
#define R_DTPOFF R_X86_64_DTPOFF64
#define R_TPOFF R_X86_64_TPOFF64
#define R_TLSDESC R_X86_64_TLSDESC
#define R_IRELATIVE R_X86_64_IRELATIVE

#elif defined(__i386__)

static constexpr uint16_t ELF_MACHINE = EM_386;

#define R_ABSOLUTE R_386_32
#define R_COPY R_386_COPY
#define R_GLOB_DAT R_386_GLOB_DAT
#define R_JUMP_SLOT R_386_JMP_SLOT
#define R_RELATIVE R_386_RELATIVE
#define R_DTPMOD R_386_TLS_DTPMOD32
#define R_DTPOFF R_386_TLS_DTPOFF32
#define R_TPOFF R_386_TLS_TPOFF
// #define R_TLSDESC R_386_TLS_DESC
#define R_IRELATIVE R_386_IRELATIVE

#else

#error missing architecture specific code

#endif
