#define BLOCK_SIZE 512 // 0.5 KB
#define NUM_BLOCKS 4096 // For some reason we're limiting ourselves to half of what we could have

void read_block(FILE* disk, int block_number, char* buffer);
void write_block(FILE* disk, int block_number, char* data);