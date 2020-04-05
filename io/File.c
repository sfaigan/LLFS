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

//https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
void set_block(unsigned char buffer[], int block_num)
{
  int index = block_num / 8;
  int bit_index = block_num % 8;

//  printf("SET\n");
//  printf("Index: %d\n", index);
//  printf("Bit Index: %d\n", bit_index);
  buffer[index] |= 1UL << bit_index;
}

void unset_block(unsigned char buffer[], int block_num)
{
  int index = block_num / 8;
  int bit_index = block_num % 8;

//  printf("UNSET\n");
//  printf("Index: %d\n", index);
//  printf("Bit Index: %d\n", bit_index);
  buffer[index] &= ~(1UL << bit_index);
}

// Create and mount the disk (I think)
void InitLLFS() {
  /* CREATE DISK */
  // Fill with 0's
  FILE* disk = fopen(vdisk_path, "w+");
  char* zeros = calloc(BLOCK_SIZE * NUM_BLOCKS, 1);
  fwrite(zeros, BLOCK_SIZE * NUM_BLOCKS, 1, disk);
  free(zeros);
  fclose(disk);

  /* FORMAT DISK */
  // Open disk
  disk = fopen(vdisk_path, "w");
  if (!disk) {
    fprintf(stderr, "ERROR: Could not open %s.\n", vdisk_path);
    exit(EXIT_FAILURE);
  }

  // Initialize buffer
  char* buffer = (char*)calloc(BLOCK_SIZE, 1);
  if (!buffer) {
    fprintf(stderr, "ERROR: Failed to allocate buffer.\n");
    exit(EXIT_FAILURE);
  }

  // Initialize Superblock
  Superblock* sb = (Superblock*)malloc(sizeof(Superblock));
  sb->magic_number = MAGIC_NUMBER;
  sb->num_blocks = NUM_BLOCKS;
  sb->num_inodes = NUM_INODES;

  // Write the superblock to the disk
  memcpy(buffer, sb, sizeof(Superblock));
  write_block(disk, 0, buffer);

  // Initialize the free block bitmap
  memset(buffer, 0, BLOCK_SIZE);
  for (int i = 0; i < 10; i++)
    set_block(buffer, i);

  // Write free block bitmap to the disk
  write_block(disk, 1, buffer);

  free(buffer);
  fclose(disk);
}