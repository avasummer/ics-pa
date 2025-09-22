#include <common.h>
#include <elf.h>
#include <stdio.h>

void init_ftrace(const char* elf) {
  Elf64_Ehdr elf_header;
  FILE *fp = fopen(elf, "rb");
  if (fp == NULL) {
    printf("Failed to open file %s\n", elf);
    return;
  }
  size_t count = fread(&elf_header, 1, sizeof(Elf64_Ehdr), fp);
  assert(count==0);
  long section_header_offset = elf_header.e_shoff;

  Elf64_Shdr section_header;
  fseek(fp, section_header_offset, SEEK_SET);

  for (int i = 0; i < elf_header.e_shnum; i++) {
  size_t count = fread(&section_header, 1, sizeof(Elf64_Shdr), fp);
    assert(count == sizeof(Elf64_Shdr));
    if (section_header.sh_type == SHT_SYMTAB) {
      printf("Symbol Table found at offset: 0x%lx, size: %lu\n",
             (long)section_header.sh_offset, (long)section_header.sh_size);
    }
  }
}