#include "util.h"

// https://stackoverflow.com/questions/47981/how-do-you-set-clear-and-toggle-a-single-bit
void set_block(unsigned char buffer[], int block_num) {
  int index = block_num / 8;
  int bit_index = block_num % 8;

//  printf("SET\n");
//  printf("Index: %d\n", index);
//  printf("Bit Index: %d\n", bit_index);
  buffer[index] |= 1UL << bit_index;
}

void unset_block(unsigned char buffer[], int block_num) {
  int index = block_num / 8;
  int bit_index = block_num % 8;

//  printf("UNSET\n");
//  printf("Index: %d\n", index);
//  printf("Bit Index: %d\n", bit_index);
  buffer[index] &= ~(1UL << bit_index);
}