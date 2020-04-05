#define vdisk_path "../disk/vdisk"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "File.h"
#include "../disk/driver.h"
#include "../disk/CONSTANTS.h"

typedef struct Superblock {
  int magic_number;
  int num_blocks;
  int num_inodes;
} Superblock;

// Create and mount the disk (I think)
void InitLLFS() {
  // Create a disk (filled with 0's)
  FILE* disk = fopen(vdisk_path, "w+");
  char* zeros = calloc(BLOCK_SIZE * NUM_BLOCKS, 1);
  fwrite(zeros, BLOCK_SIZE * NUM_BLOCKS, 1, disk);
  free(zeros);
  fclose(disk);

  // Initialize Block 0 (Superblock)
  disk = fopen(vdisk_path, "w");
  if (!disk) {
    fprintf(stderr, "ERROR: Could not open %s.\n", vdisk_path);
    exit(EXIT_FAILURE);
  }
  char* buffer = (char*)calloc(BLOCK_SIZE, 1);

  if (!buffer) {
    fprintf(stderr, "ERROR: Failed to allocate buffer.\n");
    exit(EXIT_FAILURE);
  }

  Superblock* sb = (Superblock*)malloc(sizeof(Superblock));

  sb->magic_number = MAGIC_NUMBER;
  sb->num_blocks = NUM_BLOCKS;
  sb->num_inodes = NUM_INODES;

  memcpy(buffer, sb, sizeof(Superblock));

  write_block(disk, 0, buffer);
  free(buffer);
  fclose(disk);
}