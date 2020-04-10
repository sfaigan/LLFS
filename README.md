# Little Log File System (LLFS)

___CSC 360 - Assigment 3___  

Shea Faigan

## Writing an App for LLFS
* Suggested: write the file in the apps/ folder
* Include File.h (in io/) for interfacing functions
* Must call `InitLLFS()` first to format the disk
* If the disk is already formatted (and it exists in the disk folder), you can call `AttachLLFS()` to mount the disk

## LLFS API Functions:
### Control
#### InitLLFS - format and mount vdisk

```void InitLLFS()```  

This function looks for a file located in `vdisk_path`, a constant in _CONSTANTS.h_ set to `"../disk/vdisk"` by default.  
If it does not exist, it will create the file. This file acts as a virtual disk that the function will then format.  
If this function is called with an existing vdisk, it will wipe it clean.   

* Blocks 0-9 are reserved.
* Block 0 is the superblock (16 bytes used)
    * bytes 0-3: magic number
    * bytes 4-7: number of blocks on disk
    * bytes 8-11: number of inodes for disk
* Block 1 is the free block vector (bitmap)
    * BLOCK_SIZE (512) bytes = 4096 bits
    * Each bit represents one block's availability
    * Free = 1
    * Full = 0
* Block 2 is the checkpoint region (inode map, or imap)
    * 2 byte indexed block numbers
    * Each index represents a file/directory
    * Each value represents the block number of latest inode for that file/directory
    * E.g. `imap[0]` would yield the block number of the root directory


#### AttachLLFS - mount existing vdisk

```void AttachLLFS()```  

This function looks for a file located in `vdisk_path`, a constant in _CONSTANTS.h_ set to `"../disk/vdisk"` by default.  
If it exists, it will mount LLFS by loading the free block bitmap and imap into memory, as well as initialize the buffer.


### FS Operations

#### execute_ls - List files and directories

```execute_ls(char* path)```

This function lists all the files and directories located at a given path.


#### execute_mkdir - Make a Directory

```int execute_mkdir(char* path, char* directory_name)```

This function creates an empty directory at a given path.


##### execute_rm - Remove a File or (Empty) Directory

```int execute_rm(char* path, char*  file_name)```

This function deletes a file or __empty__ directory at a given path.


#### execute_touch - Create an empty file

```int execute_touch(char* path, char* file_name)```

This function creates an empty file at a given path.


#### write_str_to_file - Write (append) to a file

```int write_str_to_file(char* path, char* file_name, char* string)```

This function appends a given string to a file determined by a given name and path.  
NOTE: this function assumes the string provided is null-terminated.


#### print_file - Print the contents of a file

```int print_file(char* path, char* file_name)```

This function prints all the data in a file determined by a given name and path.


## Future Work
* Implement garbage collection
    * [Garbage collection was not required for this assignment](https://coursespaces.uvic.ca/mod/hsuforum/discuss.php?d=82627#p140550)
    * Free block vector must be working and blocks marked as free when they are outdated (this has been implemented).
* Write more tests
* Interface with SEEsh
* Raise directory capacity
    * This would mean allowing directories to have multiple blocks with up to 16 entries each
* Implement editing existing data in a file (currently write_str_to_file() just allows appending)
    * [Editing/deleting already written blocks in a data file was not required for this assignment](https://coursespaces.uvic.ca/mod/hsuforum/discuss.php?d=82627#p142343)
    * Extension of this is to allow the last block, if partially filled, to be appended to before writing a whole new block
* Rewrite file reading logic
    * Current implementation is uses a looped putchar() because it must handle partially filled blocks. Partially filled blocks have 0s in the empty bytes, meaning we can't use printf (e.g. file with blocks following the partially filled block would not be printed because the 0 would be handled as a null terminator).
* Make tests more concise


## Known Issues
* If the disk fills up, all outdated blocks will be marked as free, but will **not** be cleared (see Future Work section).
