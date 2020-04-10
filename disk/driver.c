#include <stdio.h>
#include "driver.h"

void read_block(FILE* disk, int block_number, char* buffer) {
  fseek(disk, block_number * BLOCK_SIZE, SEEK_SET);
  fread(buffer, BLOCK_SIZE, 1, disk);
}

void write_block(FILE* disk, int block_number, char* data) {
  fseek(disk, block_number * BLOCK_SIZE, SEEK_SET);
  fwrite(data, BLOCK_SIZE, 1, disk);
}