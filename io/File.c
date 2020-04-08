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

int get_first_data_block_from_inode_index(int inode_index, char* block) {
  BLK_NO inode_blk_no = imap[inode_index];
  if (inode_blk_no == 0) {
    fprintf(stderr, "ERROR: Unable to find inode from inode index %d.\n", inode_index);
    return ERROR;
  }
  INode *inode = (INode *) malloc(sizeof(INode));
  BLK_NO data_block_no;
  // INode block is in the buffer segment
  if (inode_blk_no > log_tail) {
    memcpy(inode, buffer+(inode_blk_no-log_tail-1)*BLOCK_SIZE, sizeof(INode));
    data_block_no = inode->direct_blocks[0];
    if (data_block_no > log_tail) {
      memcpy(block, buffer+(data_block_no-log_tail-1)*BLOCK_SIZE, BLOCK_SIZE);
    } else {
      FILE *disk = fopen(vdisk_path, "rb+");
      read_block(disk, data_block_no, block);
      fclose(disk);
    }
  } else {
    FILE *disk = fopen(vdisk_path, "rb+");
    char *buf = calloc(BLOCK_SIZE, 1);
    read_block(disk, inode_blk_no, buf);
    memcpy(inode, buf, sizeof(INode));
    data_block_no = inode->direct_blocks[0];
    if (data_block_no > log_tail) {
      memcpy(block, buffer + (data_block_no-log_tail-1) * BLOCK_SIZE, BLOCK_SIZE);
    } else {
      read_block(disk, inode->direct_blocks[0], block);
    }
    fclose(disk);
    free(buf);
  }
  free(inode);
  return SUCCESS;
}

int add_file_to_dir(char* file_name, INODE_NO inode_index, DirectoryEntry* parent_dir) {
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
  INODE_NO parent_index = 0; // root
  BLK_NO parent_inode_block_no = imap[parent_index];
  int i;

  char directory_path[strlen(path)+1];
  strcpy(directory_path, path);
  char* token = strtok(directory_path, "/");
  if (token == NULL)
    return parent_index; // root inode index

  FILE* disk = fopen(vdisk_path, "rb+");
  char* block = calloc(BLOCK_SIZE, 1);
  INode* parent_inode = (INode*)malloc(sizeof(INode));
  int parent_dir_data_block_no;
  DirectoryEntry* parent_dir = (DirectoryEntry*)malloc(sizeof(DirectoryEntry)*DIRECTORY_CAPACITY);

  while (token != NULL) {
    // Parent inode is in buffer segment
    if (parent_inode_block_no > log_tail) {
      memcpy(parent_inode, buffer+(parent_inode_block_no-log_tail-1)*BLOCK_SIZE, sizeof(INode));
    } else {
      read_block(disk, parent_inode_block_no, block);
      memcpy(parent_inode, block, sizeof(INode));
      memset(block, 0, BLOCK_SIZE);
    }
    parent_dir_data_block_no = parent_inode->direct_blocks[0];
    if (parent_dir_data_block_no > log_tail) {
      memcpy(parent_dir, buffer + (parent_dir_data_block_no - log_tail - 1) * BLOCK_SIZE, BLOCK_SIZE);
    } else {
      read_block(disk, parent_dir_data_block_no, block);
      memcpy(parent_dir, block, BLOCK_SIZE);
      memset(block, 0, BLOCK_SIZE);
    }
    for (i = 0; i < DIRECTORY_CAPACITY; i++) {
      if (!strcmp(token, parent_dir[i].file_name)) {
        parent_index = parent_dir[i].inode_index;
        parent_inode_block_no = imap[parent_index];
        break;
      }
    }
    if (i == DIRECTORY_CAPACITY) {
      fprintf(stderr, "ERROR: Unable to resolve path.\n");
      free(block);
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
      memcpy(directory[i].file_name, "test", sizeof("test"));
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

  // Initialize i-node
  INode* new_inode = (INode*)malloc(sizeof(INode));
  new_inode->size = 0; // TODO: Implement file size
  new_inode->flags = type;
  new_inode->direct_blocks[0] = log_tail+segment_tail+1; // the block number of the data block
  memcpy(block, new_inode, sizeof(INode));
  free(new_inode);

  // Write i-node to buffer
  if (allocate_block(block) == ERROR) {
    fprintf(stderr, "Unable to allocate a new block for the i-node.\n");
    return ERROR;
  }
  memset(block, 0, BLOCK_SIZE);

  imap[free_inode_index] = log_tail+segment_tail+1; // the block number of the i-node

  // If the new file was not the root directory (there is a path)
  if (strlen(path)) {
    // Create a new data block for the parent directory
    DirectoryEntry parent_dir[16]; //= (DirectoryEntry *)malloc(sizeof(DirectoryEntry) * DIRECTORY_CAPACITY);
    get_first_data_block_from_inode_index(parent_dir_inode_index, block);
    memcpy(parent_dir, block, BLOCK_SIZE);
    memset(block, 0, BLOCK_SIZE);
    add_file_to_dir(file_name, free_inode_index, parent_dir);
    memcpy(block, parent_dir, BLOCK_SIZE);
    //free(parent_dir);

    // Write data block to buffer
    if (allocate_block(block) == ERROR) {
      fprintf(stderr, "ERROR: Unable to allocate a new block for data.\n");
      return ERROR;
    }
    memset(block, 0, BLOCK_SIZE);

    INode* new_parent_dir_inode = (INode*)malloc(sizeof(INode));
    new_parent_dir_inode->size = 0; // TODO: Implement file size
    new_parent_dir_inode->flags = DIRECTORY;
    new_parent_dir_inode->direct_blocks[0] = log_tail+segment_tail+1; // the block number of the data block
    memcpy(block, new_parent_dir_inode, sizeof(INode));
    free(new_parent_dir_inode);

    // Write i-node to buffer
    if (allocate_block(block) == ERROR) {
      fprintf(stderr, "Unable to allocate a new block for the i-node.\n");
      return ERROR;
    }
    memset(block, 0, BLOCK_SIZE);

    imap[parent_dir_inode_index] = log_tail+segment_tail+1; // the block number of the i-node
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
  get_first_data_block_from_inode_index(parent_dir_inode_index, block);
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

int main() {
  InitLLFS();
  create_file("/", "abc", DATA_FILE);
//  write_segment_to_disk();
  create_file("/", "test", DATA_FILE);
//  write_segment_to_disk();
  create_file("/", "foo", DIRECTORY);
//  write_segment_to_disk();
  create_file("/foo", "bar", DIRECTORY);
  create_file("/foo/bar", "baz", DATA_FILE);
  execute_ls("");
  printf("\n");
  execute_ls("/foo");
  printf("\n");
  execute_ls("/foo/bar");
  return 0;
}