# Little Log File System (LLFS)

___CSC 360 - Assigment 3___  

Shea Faigan

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
    * Current implementation is uses a looped putchar() because it must handle partially filled blocks. Partially filled blocks have 0s in the empty bytes, meaning we can't use printf (e.g. file with blocks following the partially filled block would not be printed because the 0 being treated as a null terminator).
* Make tests more concise

## Known Issues
* If the disk fills up, all outdated blocks will be marked as free, but will not be cleared (see Future Work section). 