#define NUM_INODES 256 // fits into 1 byte
#define MAGIC_NUMBER 4214869 // "abracadabra" as an int
#define SEGMENT_SIZE 16 // buffer size (in blocks)
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
#define FREE 1
#define FULL 0