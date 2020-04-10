#define vdisk_path "../disk/vdisk"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../io/CONSTANTS.h"
#include "../disk/driver.h"
#include "../io/File.h"

// Goal 1: Can manipulate a simulated disk and format that disk with your version of LLFS
// Checks if the superblock and free block bitmap were initialized correctly
int test_goal1() {
  printf("TESTING GOAL 1: Creating and mounting a disk\n"
         "============================================\n");
  InitLLFS();
  FILE* disk = fopen(vdisk_path, "rb");
  char* buffer = (char*)malloc(BLOCK_SIZE);
  read_block(disk, 0, buffer);

  int magic_number = MAGIC_NUMBER;
  int num_blocks = NUM_BLOCKS;
  int num_inodes = NUM_INODES;

  int magic_number_present = memcmp(buffer, &magic_number, sizeof(magic_number));
  int num_blocks_present = memcmp(buffer + sizeof(magic_number), &num_blocks, sizeof(num_blocks));
  int num_inodes_present = memcmp(buffer + sizeof(magic_number) + sizeof(num_blocks), &num_inodes, sizeof(num_inodes));

  read_block(disk, 1, buffer);
  unsigned char* expected_bitmap = malloc(BLOCK_SIZE);
  memset(expected_bitmap, 0xFF, BLOCK_SIZE);
  memset(expected_bitmap, 0x00, 1); // blocks 0-7 reserved
  memset(expected_bitmap+1, 0xf0, 1); // blocks 8-9: reserved; blocks 10-11: root directory
  int bitmap_present = memcmp(buffer, expected_bitmap, BLOCK_SIZE);

  free(expected_bitmap);
  free(buffer);
  fclose(disk);

  if (magic_number_present == 0 && num_blocks_present == 0 && num_blocks_present == 0 && bitmap_present == 0) {
    printf("GOAL 1: PASS\n\n");
    return 1;
  } else {
    printf("GOAL 1: FAIL\n\n");
    if (magic_number_present != 0)
      printf("Magic number not contained in first 4 bytes of superblock.\n");
    if (num_blocks_present != 0)
      printf("Number of blocks not contained in second 4 bytes of superblock.\n");
    if (num_inodes_present != 0)
      printf("Number of i-nodes not contained in third 4 bytes of superblock.\n");
    if (bitmap_present != 0)
      printf("Free block vector not initialized correctly.\n");
    return 0;
  }
}

int main() {
  int goal1 = test_goal1();

  int total_tests   = 1;
  int test_passes   = goal1;
  int test_failures = total_tests - test_passes;

  printf("\nTEST02 - REPORT\n"
         "==========================================\n");
  printf("Total number of tests: %d\n", total_tests);
  printf("Tests passed: %d\n", test_passes);
  printf("Tests failed: %d\n", test_failures);
  printf("==========================================\n\n");

  return EXIT_SUCCESS;
}