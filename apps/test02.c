#define vdisk_path "../disk/vdisk"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../disk/CONSTANTS.h"
#include "../disk/driver.h"
#include "../io/File.h"

void test_InitLLFS() {
  InitLLFS();
  FILE* disk = fopen(vdisk_path, "r+");
  char* buffer = (char*)malloc(BLOCK_SIZE);
  readBlock(disk, 0, buffer);
  fclose(disk);

  int magic_number = MAGIC_NUMBER;
  int num_blocks = NUM_BLOCKS;
  int num_inodes = NUM_INODES;

  int magic_number_present = memcmp(buffer, &magic_number, sizeof(magic_number) != 0);
  int num_blocks_present = memcmp(buffer + sizeof(magic_number), &num_blocks, sizeof(num_blocks) != 0);
  int num_inodes_present = memcmp(buffer + sizeof(magic_number) + sizeof(num_blocks), &num_inodes, sizeof(num_inodes) != 0);

  free(buffer);



  if (magic_number_present == 0 && num_blocks_present == 0 && num_blocks_present == 0) {
    printf("PASS\n");
  } else {
    printf("FAILURE:\n");
    if (magic_number_present != 0)
      printf("Magic number not contained in first 4 bytes of superblock.\n");
    if (num_blocks_present != 0)
      printf("Number of blocks not contained in second 4 bytes of superblock.\n");
    if (num_inodes_present != 0)
      printf("Number of i-nodes not contained in third 4 bytes of superblock.\n");
  }
}

void main() {
  test_InitLLFS();
}