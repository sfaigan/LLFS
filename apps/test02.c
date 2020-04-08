#define vdisk_path "../disk/vdisk"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../io/CONSTANTS.h"
#include "../disk/driver.h"
#include "../io/File.h"

void test_InitLLFS() {
  InitLLFS();
  FILE* disk = fopen(vdisk_path, "r+");
  char* buffer = (char*)malloc(BLOCK_SIZE);
  read_block(disk, 0, buffer);

  int magic_number = MAGIC_NUMBER;
  int num_blocks = NUM_BLOCKS;
  int num_inodes = NUM_INODES;

  int magic_number_present = memcmp(buffer, &magic_number, sizeof(magic_number));
  int num_blocks_present = memcmp(buffer + sizeof(magic_number), &num_blocks, sizeof(num_blocks));
  int num_inodes_present = memcmp(buffer + sizeof(magic_number) + sizeof(num_blocks), &num_inodes, sizeof(num_inodes));

  read_block(disk, 1, buffer);
  int bitmap = 0x03FF;
  int bitmap_present = memcmp(buffer, &bitmap, sizeof(bitmap));

  if (magic_number_present == 0 && num_blocks_present == 0 && num_blocks_present == 0 && bitmap_present == 0) {
    printf("PASS\n");
  } else {
    printf("FAILURE:\n");
    if (magic_number_present != 0)
      printf("Magic number not contained in first 4 bytes of superblock.\n");
    if (num_blocks_present != 0)
      printf("Number of blocks not contained in second 4 bytes of superblock.\n");
    if (num_inodes_present != 0)
      printf("Number of i-nodes not contained in third 4 bytes of superblock.\n");
    if (bitmap_present != 0)
      printf("Free block vector not initialized correctly.\n");
  }
  free(buffer);
  fclose(disk);
}

//void main() {
//  test_InitLLFS();
//}