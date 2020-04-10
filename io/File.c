#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "File.h"
#include "util.h"
#include "../disk/driver.h"

char buffer[BLOCK_SIZE*SEGMENT_SIZE];
unsigned char free_block_bitmap[BLOCK_SIZE];
BLK_NO imap[NUM_INODES];

int log_tail; // keeps track of the last block in the log
int segment_tail; // keeps track of the last block in the segment

void get_directory_entries_from_inode(INode* inode, DirectoryEntry dir[]){
  char* block = calloc(BLOCK_SIZE, 1);
  get_data_block_from_inode(inode, 0, block);
  memcpy(dir, block, sizeof(DirectoryEntry)*DIRECTORY_CAPACITY);
  free(block);
}

void get_directory_entries_from_inode_index(int inode_index, DirectoryEntry dir[]) {
  INode* inode = (INode*)malloc(sizeof(INode));
  get_inode(inode_index, inode);
  get_directory_entries_from_inode(inode, dir);
  free(inode);
}

int get_log_tail_from_free_block_bitmap() {
  int i, j, k;
  for (i = NUM_BLOCKS - 1; i >= 0; i--) {
    j = i / 8;
    k = i % 8;
    if (!(free_block_bitmap[j] & (1 << k)))
      return i;
  }
  return -1;
}

int write_segment_to_disk() {
  FILE* disk = fopen(vdisk_path, "rb+");
  char* block = calloc(BLOCK_SIZE, 1);
  for (int i = 0; i <= segment_tail; i++) {
    log_tail++;
    write_block(disk, log_tail, buffer+(BLOCK_SIZE*i));
  }
  memcpy(block, free_block_bitmap, BLOCK_SIZE);
  write_block(disk, FREE_BLOCK_BITMAP_INDEX, block);
  memset(block, 0, BLOCK_SIZE);
  memcpy(block, imap, sizeof(imap));
  write_block(disk, IMAP_BLOCK_INDEX, block);
  segment_tail = -1;
  memset(buffer, 0, BLOCK_SIZE*SEGMENT_SIZE);
  free(block);
  fclose(disk);
  return SUCCESS;
}

int allocate_block(void* data) {
  // must have room in the buffer
  if (segment_tail >= SEGMENT_SIZE-1) {
    int status = write_segment_to_disk();
    if (status == ERROR) {
      fprintf(stderr, "Failed to write buffer segment to disk.\n");
      return ERROR;
    }
    segment_tail = -1;
  }
  segment_tail++;
  memcpy(buffer + segment_tail*BLOCK_SIZE, data, BLOCK_SIZE);
  set_block_full(free_block_bitmap, log_tail+segment_tail+1);
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
  return SUCCESS;
}

int get_inode(int inode_index, INode* inode) {
  BLK_NO inode_blk_no = imap[inode_index];
  if (!inode_blk_no) {
    fprintf(stderr, "ERROR: Unable to find inode from inode index %d.\n", inode_index);
    return ERROR;
  }
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
  return inode_blk_no;
}

int get_data_block_from_inode(INode* inode, int block_index, char* block) {
  int data_block_no = inode->direct_blocks[block_index];
  if (!data_block_no) {
    return ERROR;
  }
  if (data_block_no > log_tail) { // Data block is in the buffer segment
    memcpy(block, buffer + (data_block_no-log_tail-1) * BLOCK_SIZE, BLOCK_SIZE);
  } else { // Data block is on disk
    FILE *disk = fopen(vdisk_path, "rb+");
    read_block(disk, data_block_no, block);
    fclose(disk);
  }
  return data_block_no;
}

int get_data_block_from_inode_index(int inode_index, int block_index, char* block) {
  INode* inode = (INode*)malloc(sizeof(INode));
  get_inode(inode_index, inode);

  int data_block_no = get_data_block_from_inode(inode, block_index, block);
  free(inode);

  return data_block_no;
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

int remove_file_from_dir(char* file_name, DirectoryEntry* parent_dir) {
  if (get_file_inode_index(file_name, parent_dir) == NOT_FOUND) {
    fprintf(stderr, "ERROR: Cannot remove the directory entry for file %s from the "
                    "provided directory because it does not exist.\n", file_name);
    return ERROR;
  }
  for (int i = 0; i < DIRECTORY_CAPACITY; i++) {
    if (!strcmp(parent_dir[i].file_name, file_name)) {
      parent_dir[i].inode_index = 0;
      strcpy(parent_dir[i].file_name, "empty"); //+1 because of null terminator
      return SUCCESS;
    }
  }
  fprintf(stderr, "ERROR: Could not remove directory entry for file %s from the provided directory.\n", file_name);
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
      fprintf(stderr, "ERROR: Unable to find %s.\n", token);
      free(block);
      free(parent_dir);
      return ERROR;
    }
    memset(block, 0, BLOCK_SIZE);
    token = strtok(NULL, "/");
  }
  free(parent_dir);
  free(block);
  return parent_index;
}

int create_file(char* path, char file_name[30], int type) {
  if (segment_tail+2 >= SEGMENT_SIZE-1)
    write_segment_to_disk();

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
  int parent_dir_inode_block_no = -1;
  int parent_dir_data_block_no = -1;
  char* block = calloc(BLOCK_SIZE, 1);
  DirectoryEntry parent_dir[DIRECTORY_CAPACITY];

  // NOTE to self:
  // If the new file was not the root directory (there is a path)
  // This IF check happens again later to continue the work with the parent directory
  // but before we do that, we have to make the file itself, so these two blocks had to be broken up
  if (strlen(path)) {
    if (segment_tail+4 >= SEGMENT_SIZE-1)
      write_segment_to_disk(); // prevent corruption
    // Get parent directory data block, store it in parent_dir
    parent_dir_data_block_no = get_data_block_from_inode_index(parent_dir_inode_index, 0, block);
    parent_dir_inode_block_no = (int)imap[parent_dir_inode_index];
    memcpy(parent_dir, block, BLOCK_SIZE);
    memset(block, 0, BLOCK_SIZE); // flush buffer
    // Try adding the file to the parent directory
    if (add_file_to_dir(file_name, free_inode_index, parent_dir) == ERROR) {
      free(block);
      fprintf(stderr, "ERROR: Could not create file %s in directory %s.\n", file_name, path);
      return ERROR;
    }
  }

  if (type == DIRECTORY) {
    DirectoryEntry* directory = (DirectoryEntry*)malloc(sizeof(DirectoryEntry)*DIRECTORY_CAPACITY);
    for (int i = 0; i < DIRECTORY_CAPACITY; i++) {
      directory[i].inode_index = 0; // 0 represents empty directory spot (and also root directory inode_index)
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
    // Write new data block for parent directory to buffer
    memcpy(block, parent_dir, BLOCK_SIZE);
    if (allocate_block(block) == ERROR) {
      //fprintf(stderr, "ERROR: Unable to allocate a new block for data.\n");
      return ERROR;
    }
    memset(block, 0, BLOCK_SIZE); // flush buffer

    // Set the parent directory's old data block to free
    if (parent_dir_data_block_no != -1)
      set_block_free(free_block_bitmap, parent_dir_data_block_no);

    BLK_NO parent_dir_inode_direct_blocks[10] = {0};
    parent_dir_inode_direct_blocks[0] = log_tail+segment_tail+1;
    allocate_inode(parent_dir_inode_index, 0, DIRECTORY, parent_dir_inode_direct_blocks);

    // Set the parent directory's old inode block to free
    if (parent_dir_inode_block_no != 0)
      set_block_free(free_block_bitmap, parent_dir_inode_block_no);

    memset(block, 0, BLOCK_SIZE);
  }
  free(block);
  return free_inode_index;
}

// Assumes string is null terminated!
int write_str_to_file(char* path, char* file_name, char* string) {
  // make sure it's not a directory
  // if file does not exist, create it
  // create as many data blocks as needed for text
  // update/create file inode
  // increase size

  // eviscerate the null terminator
  char text[strlen(string)];
  memcpy(text, string, strlen(string));

  // root directory or invalid path
  if ((!strlen(path) && !strlen(file_name)) || !strcmp(file_name, ROOT_DIR)) {
    fprintf(stderr, "ERROR: Path invalid.\n");
    return ERROR;
  }

  // check to see if path is valid
  int parent_dir_inode_index = get_leaf_dir_inode_index(path);
  if (parent_dir_inode_index == ERROR) {
    fprintf(stderr, "ERROR: Cannot resolve path %s\n", path);
    return ERROR;
  }

  //int num_blocks_needed = ceil(((double)strlen(text)+1)/512);
  int num_blocks_needed = ceil((double)sizeof(text)/512);
  if (num_blocks_needed > 10) {
    fprintf(stderr, "ERROR: Text is too long. Indirect blocks not supported, so text needs to be less than 5120 bytes.\n");
    return ERROR;
  }
  int file_inode_index;
  int i, j, num_blocks_occupied = 0;
  char* block = calloc(BLOCK_SIZE, 1);
  DirectoryEntry parent_dir[DIRECTORY_CAPACITY];
  INode* file_inode = (INode*)malloc(sizeof(INode));
  BLK_NO data_block_numbers[10] = {0};

  // Get parent directory data block, store it in parent_dir
  get_data_block_from_inode_index(parent_dir_inode_index, 0, block);
  memcpy(parent_dir, block, BLOCK_SIZE);
  memset(block, 0, BLOCK_SIZE); // flush buffer
  file_inode_index = get_file_inode_index(file_name, parent_dir);
  if (file_inode_index == NOT_FOUND) {
    file_inode_index = create_file(path, file_name, DATA_FILE);
  }
  get_inode(file_inode_index, file_inode);

  for (i = 0; i < 10; i++) {
    if (file_inode->direct_blocks[i] != 0) {
      data_block_numbers[i] = file_inode->direct_blocks[i];
      num_blocks_occupied++;
    }
  }
  if (num_blocks_occupied == 1) {
    char* zeros = calloc(BLOCK_SIZE, 1);
    get_data_block_from_inode(file_inode, 0, block);
    if (!memcmp(zeros, block, BLOCK_SIZE))
      num_blocks_occupied = 0; // first block is actually empty
    free(zeros);
    memset(block, 0, BLOCK_SIZE);
  }
  free(file_inode);

  if (num_blocks_needed > (10 - num_blocks_occupied)) {
    fprintf(stderr, "ERROR: File already has data in it, and you've provided too much to fit in the remaining free direct blocks.\n");
    return ERROR;
  }

  int bytes_of_text;
  int bytes_of_text_remaining;

  for (i = num_blocks_occupied, j = 0; i < 10 && j < num_blocks_needed; i++, j++) {
    bytes_of_text = BLOCK_SIZE;
    bytes_of_text_remaining = sizeof(text)-(j*BLOCK_SIZE);// +1;
    if (bytes_of_text_remaining < BLOCK_SIZE)
      bytes_of_text = bytes_of_text_remaining;
    memcpy(block, &text[j*BLOCK_SIZE], bytes_of_text);
    if (allocate_block(block) == ERROR) {
      fprintf(stderr, "ERROR: Unable to allocate a new block for data.\n");
      return ERROR;
    }
    memset(block, 0, BLOCK_SIZE);
    data_block_numbers[i] = log_tail+segment_tail+1;
  }
  free(block);

  // Create i-node
  return allocate_inode(file_inode_index, 0, DATA_FILE, data_block_numbers);
}

int read_file(char* path, char* file_name, char* buf) {
  if ((!strlen(path) && !strlen(file_name)) || !strcmp(file_name, ROOT_DIR)) {
    fprintf(stderr, "ERROR: Path invalid.\n");
    return ERROR;
  }

  // check to see if path is valid
  int parent_dir_inode_index = get_leaf_dir_inode_index(path);
  if (parent_dir_inode_index == ERROR) {
    fprintf(stderr, "ERROR: Cannot resolve path %s\n", path);
    return ERROR;
  }

  int file_inode_index;
  int i;
  DirectoryEntry parent_dir[DIRECTORY_CAPACITY];
  char* block = calloc(BLOCK_SIZE, 1);

  // Get parent directory data block, store it in parent_dir
  get_data_block_from_inode_index(parent_dir_inode_index, 0, block);
  memcpy(parent_dir, block, BLOCK_SIZE);
  free(block);
  file_inode_index = get_file_inode_index(file_name, parent_dir);
  if (file_inode_index == NOT_FOUND) {
     fprintf(stderr, "ERROR: Could not locate file %s in path %s.\n", file_name, path);
     return ERROR;
  }
  INode* file_inode = (INode*)malloc(sizeof(INode));
  get_inode(file_inode_index, file_inode);
  int blocks_full = 0;

  for (i = 0; i < 10; i++) {
    if (file_inode->direct_blocks[i] != 0) {
      blocks_full++;
      get_data_block_from_inode(file_inode, i, buf+(i*BLOCK_SIZE));
    }
  }
  free(file_inode);
  return blocks_full;
}

int delete_file(char* path, char* file_name) {
  // Get parent directory i-node index
  int parent_dir_inode_index = get_leaf_dir_inode_index(path);
  if (parent_dir_inode_index == ERROR) {
    fprintf(stderr, "ERROR: Could not delete file %s in directory %s.\n", file_name, path);
    return ERROR;
  }
  char *block = calloc(BLOCK_SIZE, 1);
  int parent_dir_inode_block_no = imap[parent_dir_inode_index];
  int file_inode_index, file_inode_block_no;

  // Get parent directory data block
  DirectoryEntry parent_dir[DIRECTORY_CAPACITY];
  int parent_dir_data_block_no = get_data_block_from_inode_index(parent_dir_inode_index, 0, block);
  memcpy(parent_dir, block, BLOCK_SIZE);
  memset(block, 0, BLOCK_SIZE);

  // Get file's i-node before we remove it from the directory
  file_inode_index = get_file_inode_index(file_name, parent_dir);
  file_inode_block_no = imap[file_inode_index];
  INode* inode = (INode*)malloc(sizeof(INode));
  get_inode(file_inode_index, inode);

  if (inode->flags == DIRECTORY) {
    DirectoryEntry dir[DIRECTORY_CAPACITY];
    get_directory_entries_from_inode(inode, dir);
    for (int i = 0; i < 10; i++) {
      if (dir[i].inode_index != 0) {
        fflush(stderr);
        fprintf(stderr, "ERROR: Cannot delete non-empty directory.\n");
        free(inode);
        free(block);
        return ERROR;
      }
    }
  }

  // Remove file from its parent directory
  if (remove_file_from_dir(file_name, parent_dir) == ERROR) {
    fprintf(stderr, "ERROR: Could not delete file %s/%s.\n", path, file_name);
    free(block);
    return ERROR;
  }
  memcpy(block, parent_dir, BLOCK_SIZE);

  // Free i-node once we know it's been removed from the parent directory
  imap[file_inode_index] = 0; // free inode index
  set_block_free(free_block_bitmap, file_inode_block_no);  // Set parent directory's old data block free

  // Free file's data blocks
  for (int i = 0; i < 10; i++)
    if (inode->direct_blocks[i] != 0)
      set_block_free(free_block_bitmap, inode->direct_blocks[i]);

  // Write new parent directory data block to buffer
  if (allocate_block(block) == ERROR) {
    fprintf(stderr, "ERROR: Unable to allocate a new block for data.\n");
    free(block);
    return ERROR;
  }
  memset(block, 0, BLOCK_SIZE);

  // Set parent directory's old data block free
  if (parent_dir_data_block_no != ERROR)
    set_block_free(free_block_bitmap, parent_dir_data_block_no);

  // Create new i-node for parent directory
  BLK_NO parent_dir_inode_direct_blocks[10] = {0};
  parent_dir_inode_direct_blocks[0] = log_tail+segment_tail+1;
  allocate_inode(parent_dir_inode_index, 0, DIRECTORY, parent_dir_inode_direct_blocks);

  // Set parent directory's old inode block free
  if (parent_dir_inode_block_no != 0)
    set_block_free(free_block_bitmap, parent_dir_inode_block_no);

  // update free block vector
  free(block);
  if (inode) free(inode);
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
  int i;

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

  // Initialize a single block buffer
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
  memset(block, 0, BLOCK_SIZE);

  // FREE BLOCK VECTOR
  // Initialize the free block bitmap
  memset(free_block_bitmap, 0xFF, BLOCK_SIZE);
  for (i = 0; i < 10; i++)
    set_block_full(free_block_bitmap, i);

  // Write free block bitmap to the disk
  memcpy(block, free_block_bitmap, BLOCK_SIZE);
  write_block(disk, FREE_BLOCK_BITMAP_INDEX, block);

  // Initialize imap
  for(i = 0; i < NUM_INODES; i++)
    imap[i] = 0; // assume 0 means free since 0 would never be the block number of an inode

  // Clean up
  log_tail = 9; // last used block
  memset(buffer, 0, BLOCK_SIZE * SEGMENT_SIZE); // flush buffer
  segment_tail = -1;
  free(sb);
  free(block);
  fclose(disk);

  create_root_directory();
}

void AttachLLFS() {
  FILE* disk = fopen(vdisk_path, "rb");
  if (!disk) {
    fprintf(stderr, "ERROR: Could not open %s.\n", vdisk_path);
    exit(EXIT_FAILURE);
  }
  // Initialize a single block buffer
  char* block = calloc(BLOCK_SIZE, 1);

  // Load free block bitmap into memory
  read_block(disk, FREE_BLOCK_BITMAP_INDEX, block);
  memcpy(free_block_bitmap, block, BLOCK_SIZE);
  memset(block, 0, BLOCK_SIZE);

  // Load imap into memory
  read_block(disk, IMAP_BLOCK_INDEX, block);
  memcpy(imap, block, BLOCK_SIZE);

  // Clean up
  free(block);
  fclose(disk);

  // Set the log and segment tails
  log_tail = get_log_tail_from_free_block_bitmap(); // last used block
  if (log_tail == -1) {
    fprintf(stderr, "ERROR: Failed to recover log tail block number.\n");
    exit(EXIT_FAILURE);
  }
  segment_tail = -1; // segment is empty

  // Flush the buffer
  memset(buffer, 0, BLOCK_SIZE * SEGMENT_SIZE);
}

// USER FACING FUNCTIONS
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

int execute_mkdir(char* path, char* directory_name) {
  return create_file(path, directory_name, DIRECTORY);
}

int execute_touch(char* path, char* file_name) {
  return create_file(path, file_name, DATA_FILE);
}

int execute_rm(char* path, char*  file_name) {
  return delete_file(path, file_name);
}

int print_file(char* path, char* file_name) {
  char* buf = calloc(BLOCK_SIZE*10, 1);
  int blocks_in_file = read_file(path, file_name, buf);
  for (int i = 0; i < blocks_in_file*BLOCK_SIZE; i++)
    if (buf[i] != 0x0)
      putchar(buf[i]);
  printf("\n");
  free(buf);
  return SUCCESS;
}