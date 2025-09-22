#include <common.h>
#include <elf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct func_table
{
  char *name;
  size_t addr;
  struct func_table* next;
} FT;

static FT* func_table_head = NULL;

void init_ftrace(const char *elf);

void ftrace_append(const char *name, size_t addr) {
  FT* p = malloc(sizeof(FT));
  if(func_table_head == NULL) func_table_head = p;
  else {
    p->next = func_table_head;
    func_table_head = p;
  }
  p->name = strdup(name);
  p->addr = addr;
}

void parse_symbols(FILE *fp, Elf64_Ehdr *elf_header) {
  Elf64_Shdr *sh_table = malloc(elf_header->e_shnum * sizeof(Elf64_Shdr));
  fseek(fp, elf_header->e_shoff, SEEK_SET);
  size_t ret = fread(sh_table, sizeof(Elf64_Shdr), elf_header->e_shnum, fp);
  assert(ret == elf_header->e_shnum);

  Elf64_Shdr *symtab_shdr = NULL;
  Elf64_Shdr *strtab_shdr = NULL;

  for (int i = 0; i < elf_header->e_shnum; i++) {
    if (sh_table[i].sh_type == SHT_SYMTAB) {
      symtab_shdr = &sh_table[i];
      if (symtab_shdr->sh_link < elf_header->e_shnum) {
        strtab_shdr = &sh_table[symtab_shdr->sh_link];
      }
      break;
    }
  }

  if (!symtab_shdr || !strtab_shdr) {
    printf("Symbol table or string table not found.\n");
    free(sh_table);
    return;
  }

  Elf64_Sym *sym_table = malloc(symtab_shdr->sh_size);
  fseek(fp, symtab_shdr->sh_offset, SEEK_SET);
  ret = fread(sym_table, symtab_shdr->sh_size, 1, fp);
  assert(ret == 1);

  char *strtab = malloc(strtab_shdr->sh_size);
  fseek(fp, strtab_shdr->sh_offset, SEEK_SET);
  ret = fread(strtab, strtab_shdr->sh_size, 1, fp);
  assert(ret == 1);


  int num_symbols = symtab_shdr->sh_size / sizeof(Elf64_Sym);
  //printf("Parsing %d symbols...\n", num_symbols);

  for (int i = 0; i < num_symbols; i++) {
    const char *symbol_name = &strtab[sym_table[i].st_name];
    Elf64_Addr symbol_addr = sym_table[i].st_value;
    unsigned char symbol_type = ELF64_ST_TYPE(sym_table[i].st_info);

    if (symbol_type == STT_FUNC) {
      printf("Found function: %s at address 0x%lx\n", symbol_name, symbol_addr);
      ftrace_append(symbol_name, symbol_addr);
    }
  }

  free(sh_table);
  free(sym_table);
  free(strtab);
}

void init_ftrace(const char* elf) {
  Elf64_Ehdr elf_header;
  FILE *fp = fopen(elf, "rb");
  if (fp == NULL) {
    printf("Failed to open file %s\n", elf);
    return;
  }
  size_t count = fread(&elf_header, 1, sizeof(Elf64_Ehdr), fp);
  assert(count == sizeof(Elf64_Ehdr));

  parse_symbols(fp, &elf_header);
}