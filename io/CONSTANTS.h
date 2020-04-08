#define BLOCK_SIZE 512 // 0.5 KB
#define NUM_BLOCKS 4096 // For some reason we're limiting ourselves to half of what we could have
#define NUM_INODES 256 // fits into 1 byte
#define MAGIC_NUMBER 4214869 // "abracadabra" as an int
#define SEGMENT_SIZE 8 // buffer size (in blocks)
#define DIRECTORY_CAPACITY 16
#define DATA_FILE 0
#define DIRECTORY 1
#define SUPERBLOCK_INDEX 0
#define FREE_BLOCK_BITMAP_INDEX 1
#define IMAP_BLOCK_INDEX 2
#define ERROR -1
#define SUCCESS 0
#define NOT_FOUND -1
#define MAX_FILE_NAME_LENGTH 30
#define ROOT_DIR "root"
#define vdisk_path "../disk/vdisk"