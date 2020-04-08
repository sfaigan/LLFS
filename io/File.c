#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "File.h"
#include "util.h"
#include "typedefs.h"
#include "CONSTANTS.h"
#include "../disk/driver.h"

char buffer[BLOCK_SIZE*SEGMENT_SIZE];
BLK_NO imap[NUM_INODES] = {0}; // assume 0 means free since 0 would never be the block number of an inode

int log_tail; // keeps track of the last block in the log
int segment_tail; // keeps track of the last block in the segment

int write_segment_to_disk() {
  FILE* disk = fopen(vdisk_path, "rb+");
  // TODO: check if there is enough room on disk
  for (int i = 0; i <= segment_tail; i++) {
    log_tail++;
    write_block(disk, log_tail, buffer+(BLOCK_SIZE*i));
    // TODO: update free block vector: set_block()
  }
  char* block = (char*)malloc(sizeof(imap));
  memcpy(block, imap, sizeof(imap));
  write_block(disk, IMAP_BLOCK_INDEX, block);
  segment_tail = -1;
  memset(buffer, 0, BLOCK_SIZE*SEGMENT_SIZE);
  fclose(disk);
  return SUCCESS;
}

int allocate_block(void* data) {
  segment_tail++;
  // must have room in the buffer
  if (segment_tail > SEGMENT_SIZE-1) {
    printf("Not enough room in buffer.\n");
    printf("Writing buffer segment to disk...\n");
    int status = write_segment_to_disk();
    if (status == ERROR) {
      fprintf(stderr, "Failed to write buffer segment to disk.\n");
      return ERROR;
    }
    segment_tail = 0;
  }
  memcpy(buffer + segment_tail*BLOCK_SIZE, data, BLOCK_SIZE);
  // TODO: mark block as full
  return SUCCESS;
}

// Returns ERROR (-1) if no free inodes (no more files allowed)
int get_free_inode_index() {
  for (int i = 0; i < NUM_INODES; i++)
    if (imap[i] == 0)
      return i;
  return ERROR;
}

int allocate_inode(INODE_NO inode_index, int size, int flags, BLK_NO* direct_blocks) {
  char* block = calloc(BLOCK_SIZE, 1);
  INode* new_inode = (INode*)malloc(sizeof(INode));
  new_inode->size = size; // TODO: Implement file size
  new_inode->flags = flags;
  memcpy(new_inode->direct_blocks, direct_blocks, sizeof(BLK_NO)*10);
  memcpy(block, new_inode, sizeof(INode));
  free(new_inode);
  // Write i-node to buffer
  if (allocate_block(block) == ERROR) {
    fprintf(stderr, "Unable to allocate a new block for the i-node.\n");
    free(block);
    return ERROR;
  }
  imap[inode_index] = log_tail+segment_tail+1; // the block number of the i-node
  free(block);
}

int get_data_block_from_inode_index(int inode_index, int block_index, char* block) {
  BLK_NO inode_blk_no = imap[inode_index];
  if (!inode_blk_no) {
    fprintf(stderr, "ERROR: Unable to find inode from inode index %d.\n", inode_index);
    return ERROR;
  }
  INode *inode = (INode *) malloc(sizeof(INode));
  BLK_NO data_block_no;

  if (inode_blk_no > log_tail) { // INode block is in the buffer segment
    memcpy(inode, buffer+(inode_blk_no-log_tail-1)*BLOCK_SIZE, sizeof(INode));
  } else { // INode block is on disk
    FILE *disk = fopen(vdisk_path, "rb+");
    char* buf = calloc(BLOCK_SIZE, 1);
    read_block(disk, inode_blk_no, buf);
    memcpy(inode, buf, sizeof(INode));
    free(buf);
    fclose(disk);
  }

  data_block_no = inode->direct_blocks[block_index];
  free(inode);
  if (!data_block_no) {
    fprintf(stderr, "ERROR: I-Node %d does not have a direct block at index %d.\n", inode_index, block_index);
    return ERROR;
  }
  if (data_block_no > log_tail) { // Data block is in the buffer segment
    memcpy(block, buffer + (data_block_no-log_tail-1) * BLOCK_SIZE, BLOCK_SIZE);
  } else { // Data block is on disk
    FILE *disk = fopen(vdisk_path, "rb+");
    read_block(disk, inode->direct_blocks[0], block);
    fclose(disk);
  }
  return SUCCESS;
}

int get_file_inode_index(char* file_name, DirectoryEntry* parent_dir) {
  for (int i = 0; i < DIRECTORY_CAPACITY; i++) {
    if (!strcmp(parent_dir[i].file_name, file_name))
      return parent_dir[i].inode_index;
  }
  return NOT_FOUND;
}

int add_file_to_dir(char* file_name, INODE_NO inode_index, DirectoryEntry* parent_dir) {
  if (get_file_inode_index(file_name, parent_dir) != NOT_FOUND) {
    fprintf(stderr, "ERROR: File %s already exists in parent directory.\n", file_name);
    return ERROR;
  }
  for (int i = 0; i < DIRECTORY_CAPACITY; i++) {
    if (!parent_dir[i].inode_index) {
      parent_dir[i].inode_index = inode_index;
      strncpy(parent_dir[i].file_name, file_name, MAX_FILE_NAME_LENGTH+1); //+1 because of null terminator
      return SUCCESS;
    }
  }
  fprintf(stderr, "ERROR: Directory is full.\n");
  return ERROR;
}

// Traverse absolute path to get inode index of the deepest directory
// Returns ERROR (-1) if for whatever reason it didn't already return with an inode index
int get_leaf_dir_inode_index(char* path) {
  INODE_NO parent_index = 0; // root inode index
  int i;

  // Tokenize path
  char directory_path[strlen(path)+1]; // strtok manipulates given string
  strcpy(directory_path, path); // need a new string to avoid segfault
  char* token = strtok(directory_path, "/");

  // If there is no token after first "/", the leaf directory is root
  if (token == NULL)
    return parent_index;

  // Initialize a block buffer
  char* block = calloc(BLOCK_SIZE, 1);

  // parent_dir will start as the root directory's data block
  DirectoryEntry* parent_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry)*DIRECTORY_CAPACITY);

  while (token != NULL) {
    get_data_block_from_inode_index(parent_index, 0, block);
    memcpy(parent_dir, block, BLOCK_SIZE);
    for (i = 0; i < DIRECTORY_CAPACITY; i++) {
      if (!strcmp(token, parent_dir[i].file_name)) {
        parent_index = parent_dir[i].inode_index;
        break;
      }
    }
    if (i == DIRECTORY_CAPACITY) {
      fprintf(stderr, "ERROR: Unable to resolve path.\n");
      free(block);
      free(parent_dir);
      return ERROR;
    }
    memset(block, 0, BLOCK_SIZE);
    token = strtok(NULL, "/");
  }
  free(block);
  return parent_index;
}

int create_file(char* path, char file_name[30], int type) {
  // check to see if there are any free inode blocks
  int free_inode_index = get_free_inode_index();
  if (free_inode_index == ERROR) {
    fprintf(stderr, "ERROR: No more files for you!\n");
    return ERROR;
  }
  // check to see if path is valid
  int parent_dir_inode_index = get_leaf_dir_inode_index(path);
  if (parent_dir_inode_index == ERROR) {
    fprintf(stderr, "ERROR: Cannot resolve path %s\n", path);
    return ERROR;
  }
  char* block = calloc(BLOCK_SIZE, 1);

  if (type == DIRECTORY) {
    DirectoryEntry* directory = (DirectoryEntry*)malloc(sizeof(DirectoryEntry)*DIRECTORY_CAPACITY);
    for (int i = 0; i < DIRECTORY_CAPACITY; i++) {
      directory[i].inode_index = 0; // 0 represents empty directory spot (and also root directory inode_index)
      memcpy(directory[i].file_name, "empty", sizeof("test"));
      //directory[i].file_name[30] = '\0';
    }
    memcpy(block, directory, BLOCK_SIZE);
    free(directory);
  }

  // Write data block to buffer
  if (allocate_block(block) == ERROR) {
    fprintf(stderr, "ERROR: Unable to allocate a new block for data.\n");
    return ERROR;
  }
  memset(block, 0, BLOCK_SIZE);

  // Create i-node
  BLK_NO data_block_numbers[10] = {0};
  data_block_numbers[0] = log_tail+segment_tail+1;
  allocate_inode(free_inode_index, 0, type, data_block_numbers);

  memset(block, 0, BLOCK_SIZE);

  // If the new file was not the root directory (there is a path)
  if (strlen(path)) {
    // Create a new data block for the parent directory
    DirectoryEntry parent_dir[16]; //= (DirectoryEntry *)malloc(sizeof(DirectoryEntry) * DIRECTORY_CAPACITY);
    get_data_block_from_inode_index(parent_dir_inode_index, 0, block);
    memcpy(parent_dir, block, BLOCK_SIZE);
    memset(block, 0, BLOCK_SIZE);
    add_file_to_dir(file_name, free_inode_index, parent_dir);
    memcpy(block, parent_dir, BLOCK_SIZE);

    // Write data block to buffer
    if (allocate_block(block) == ERROR) {
      fprintf(stderr, "ERROR: Unable to allocate a new block for data.\n");
      return ERROR;
    }
    memset(block, 0, BLOCK_SIZE);

    BLK_NO parent_dir_inode_direct_blocks[10] = {0};
    parent_dir_inode_direct_blocks[0] = log_tail+segment_tail+1;
    allocate_inode(parent_dir_inode_index, 0, DIRECTORY, parent_dir_inode_direct_blocks);

    memset(block, 0, BLOCK_SIZE);
  }

  free(block);
}

int delete_file() {
  // imap[inode_id] = -1;
  // update free block vector
  return 0;
}

int create_root_directory() {
  create_file("", ROOT_DIR, DIRECTORY);
  write_segment_to_disk();
  return 0;
}

int create_disk() {
  FILE* disk = fopen(vdisk_path, "w+");
  char* zeros = calloc(BLOCK_SIZE * NUM_BLOCKS, 1);
  fwrite(zeros, BLOCK_SIZE * NUM_BLOCKS, 1, disk);
  free(zeros);
  fclose(disk);
  return EXIT_SUCCESS;
}

// Create and mount the disk (I think)
void InitLLFS() {
  /* CREATE DISK */
  // Fill with 0's
  if (create_disk() == ERROR) {
    fprintf(stderr, "ERROR: Unable to create disk.\n");
    exit(EXIT_FAILURE);
  }

  /* FORMAT DISK */
  // Open disk
  FILE* disk = fopen(vdisk_path, "rb+");
  if (!disk) {
    fprintf(stderr, "ERROR: Could not open %s.\n", vdisk_path);
    exit(EXIT_FAILURE);
  }

  char* block = calloc(BLOCK_SIZE, 1);

  // SUPERBLOCK
  // Initialize Superblock
  Superblock* sb = (Superblock*)malloc(sizeof(Superblock));
  sb->magic_number = MAGIC_NUMBER;
  sb->num_blocks = NUM_BLOCKS;
  sb->num_inodes = NUM_INODES;

  // Write the superblock to the disk
  memcpy(block, sb, sizeof(Superblock));
  write_block(disk, SUPERBLOCK_INDEX, block);

  // FREE BLOCK VECTOR
  // Initialize the free block bitmap
  memset(block, 0, BLOCK_SIZE);
  for (int i = 0; i < 10; i++)
    set_block(block, i);

  // Write free block bitmap to the disk
  write_block(disk, FREE_BLOCK_BITMAP_INDEX, block);

  // Clean up
  log_tail = 9; // next usable block
  memset(buffer, 0, BLOCK_SIZE * SEGMENT_SIZE); // flush buffer
  segment_tail = -1;

  create_root_directory();

  fclose(disk);
  free(block);
}

int execute_ls(char* path) {
  // check to see if path is valid
  int parent_dir_inode_index = get_leaf_dir_inode_index(path);
  if (parent_dir_inode_index == ERROR) {
    fprintf(stderr, "ERROR: Cannot resolve path %s\n", path);
    return ERROR;
  }
  printf("PATH: %s/\n", path);
  char* block = calloc(BLOCK_SIZE, 1);
  get_data_block_from_inode_index(parent_dir_inode_index, 0, block);
  DirectoryEntry* parent_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry)*DIRECTORY_CAPACITY);
  memcpy(parent_dir, block, BLOCK_SIZE);
  for (int i = 0; i < DIRECTORY_CAPACITY; i++) {
    if (parent_dir[i].inode_index != 0)
      printf("%s\t", parent_dir[i].file_name);
  }
  printf("\n");
  free(block);
  free(parent_dir);
  return SUCCESS;
}

// TODO: Duplicate file name checks
int main() {
  InitLLFS();
  create_file("/", "abc", DATA_FILE);
  create_file("/", "test", DATA_FILE);
  create_file("/", "foo", DIRECTORY);
  create_file("/foo", "bar", DIRECTORY);
  create_file("/foo/bar", "baz", DATA_FILE);
  execute_ls("");
  printf("\n");
  execute_ls("/foo");
  printf("\n");
  execute_ls("/foo/bar");
  return 0;
}