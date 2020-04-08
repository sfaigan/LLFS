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

## Known Issues
* If the disk fills up, all outdated blocks will be marked as free, but will not be cleared (see Future Work section). 