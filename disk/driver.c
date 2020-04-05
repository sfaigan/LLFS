#include <stdio.h>
#include "driver.h"
#include "CONSTANTS.h"

void readBlock(FILE* disk, int block_number, char* buffer) {
  fseek(disk, block_number * BLOCK_SIZE, SEEK_SET);
  fread(buffer, BLOCK_SIZE, 1, disk);
}

void writeBlock(FILE* disk, int block_number, char* data) {
  fseek(disk, block_number * BLOCK_SIZE, SEEK_SET);
  fwrite(data, BLOCK_SIZE, 1, disk);
}