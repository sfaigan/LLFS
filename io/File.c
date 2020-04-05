#define vdisk_path "../disk/vdisk"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "File.h"
#include "../disk/driver.h"
#include "../disk/CONSTANTS.h"


void InitLLFS() {
  // Open the disk and fill it with zeros
  FILE* disk = fopen(vdisk_path, "w+");
  char* zeros = calloc(BLOCK_SIZE * NUM_BLOCKS, 1);
  fwrite(zeros, BLOCK_SIZE * NUM_BLOCKS, 1, disk);
  free(zeros);
  fclose(disk);

  // Initialize Block 0 (Superblock)
  disk = fopen(vdisk_path, "w");
  char* buffer = (char*)calloc(BLOCK_SIZE, 1);
  int magic_number = MAGIC_NUMBER;
  int num_blocks = NUM_BLOCKS;
  int num_inodes = NUM_INODES;
  memcpy(buffer, &magic_number, sizeof(magic_number));
  memcpy(buffer + sizeof(int), &num_blocks, sizeof(int));
  memcpy(buffer + 2*sizeof(int), &num_inodes, sizeof(int));
  writeBlock(disk, 0, buffer);
  free(buffer);
  fclose(disk);
}