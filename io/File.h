#include "typedefs.h"
#include "CONSTANTS.h"

void get_directory_entries_from_inode(INode* inode, DirectoryEntry dir[]);
void get_directory_entries_from_inode_index(int inode_index, DirectoryEntry dir[]);
int get_log_tail_from_free_block_bitmap();
int write_segment_to_disk();
int allocate_block(void* data);
int get_free_inode_index();
int allocate_inode(INODE_NO inode_index, int size, int flags, BLK_NO* direct_blocks);
int get_inode(int inode_index, INode* inode);
int get_data_block_from_inode(INode* inode, int block_index, char* block);
int get_data_block_from_inode_index(int inode_index, int block_index, char* block);
int get_file_inode_index(char* file_name, DirectoryEntry* parent_dir);
int add_file_to_dir(char* file_name, INODE_NO inode_index, DirectoryEntry* parent_dir);
int remove_file_from_dir(char* file_name, DirectoryEntry* parent_dir);
int get_leaf_dir_inode_index(char* path);
int create_file(char* path, char name[30], int type);
int write_str_to_file(char* path, char* file_name, char* string);
int read_file(char* path, char* file_name, char* buf);
int delete_file(char* path, char* file_name);
int create_root_directory();
int create_disk();
void InitLLFS();
void RecoverLLFS();
int execute_ls(char* path);
int execute_mkdir(char* path, char* directory_name);
int execute_rm(char* path, char*  file_name);
int print_file(char* path, char* file_name);