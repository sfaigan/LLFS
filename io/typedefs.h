// assignment spec states block number must be 2 bytes
// so this can actually support up to 65536 blocks
typedef unsigned short BLK_NO;

// only 256
typedef unsigned char INODE_NO;

// Size: 12 bytes
typedef struct Superblock {
  int magic_number;
  int num_blocks;
  int num_inodes;
} Superblock;

// Size: 32 bytes
typedef struct INode {
  int size;
  int flags;
  BLK_NO direct_blocks[10];
  BLK_NO single_indirect_block; // bonus marks if implemented
  BLK_NO double_indirect_block; // bonus marks if implemented
} INode;

// Size: 32 bytes
typedef struct DirectoryEntry {
  INODE_NO inode_index;
  char file_name[31];
} DirectoryEntry;