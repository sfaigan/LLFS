#include "util.h"

// https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
// Set the bit to 1
void set_block_free(unsigned char buffer[], int block_num) {
  int index = block_num / 8;
  int bit_index = block_num % 8;

//  printf("SET\n");
//  printf("Index: %d\n", index);
//  printf("Bit Index: %d\n", bit_index);
  buffer[index] |= 1UL << bit_index;
}

// Set the bit to 0
void set_block_full(unsigned char buffer[], int block_num) {
  int index = block_num / 8;
  int bit_index = block_num % 8;

//  printf("UNSET\n");
//  printf("Index: %d\n", index);
//  printf("Bit Index: %d\n", bit_index);
  buffer[index] &= ~(1UL << bit_index);
}