#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../io/CONSTANTS.h"
#include "../disk/driver.h"
#include "../io/File.h"

// Goal 2a: Reading and writing of files in the root directory
// ====================================================================
// Creates 3 test files: file1, file2, file3
// Writes 1 block of text to file 1, then appends another
// Writes 5 blocks of text to file2
// Writes 10 blocks of text to file10
// Reads and compares the contents of each file with the text that was written to them
int test_goal2a() {
  printf("TESTING GOAL 2a: Reading and writing files in the root directory\n"
         "================================================================\n");
  InitLLFS();
  printf("Creating file1...\n");
  execute_touch("/", "file1");
  printf("Creating file2...\n");
  execute_touch("/", "file2");
  printf("Creating file3...\n");
  execute_touch("/", "file3");
  printf("\nExecuting ls in root directory...\n");
  execute_ls("");
  printf("\n");

  // Flags -- 0 means pass (memcmp)

  int file1a_test = -1; // we can write one block to a file
  int file1b_test = -1; // we can append a block to a file with existing data
  int file2_test = -1; // we can write five blocks to a file
  int file3_test = -1; // we can write ten blocks to a file

  // 10 because that's the max number of directed blocks
  char* buf = calloc(10*BLOCK_SIZE, 1);

  // Initialize data to be written to our files
  char* file1_text_1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut in massa a augue fringilla condimentum rhoncus nec augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur vitae efficitur nibh. In commodo mauris posuere ipsum ullamcorper rutrum. Cras non nulla in justo imperdiet placerat. Mauris vehicula ultricies elementum. Proin suscipit vitae nisl eget pretium. Aenean in lacinia arcu. Phasellus nec ex eget dui accumsan consectetur a in nisi. Suspendisse";
  char* file1_text_2 = " lobortis ante ut euismod pellentesque. Nulla facilisi. Ut et euismod libero. Cras porta tempor diam et tincidunt. Donec et nibh aliquet, ultricies lorem ut, lacinia mi. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed nibh odio, sodales a nunc vel, vestibulum dictum dui. Sed faucibus mollis elit, eget luctus odio posuere tincidunt. Duis non elit lectus. Etiam eget lobortis lorem.";
  char* file2_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut in massa a augue fringilla condimentum rhoncus nec augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur vitae efficitur nibh. In commodo mauris posuere ipsum ullamcorper rutrum. Cras non nulla in justo imperdiet placerat. Mauris vehicula ultricies elementum. Proin suscipit vitae nisl eget pretium. Aenean in lacinia arcu. Phasellus nec ex eget dui accumsan consectetur a in nisi. Suspendisse lobortis ante ut euismod pellentesque. Nulla facilisi. Ut et euismod libero. Cras porta tempor diam et tincidunt. Donec et nibh aliquet, ultricies lorem ut, lacinia mi. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed nibh odio, sodales a nunc vel, vestibulum dictum dui. Sed faucibus mollis elit, eget luctus odio posuere tincidunt. Duis non elit lectus. Etiam eget lobortis lorem. Donec feugiat, mi sed blandit laoreet, leo orci laoreet nisi, non viverra diam sapien porttitor dolor. Donec porttitor orci at risus ullamcorper ornare. Fusce faucibus faucibus sapien vel porttitor. Vivamus urna mi, laoreet sagittis dignissim vel, imperdiet at turpis. Duis quis purus condimentum, ornare tellus sed, vestibulum sem. Aliquam leo risus, aliquet ac lectus malesuada, laoreet consectetur massa. Ut ante turpis, tempor vel justo ut, volutpat imperdiet felis. Cras non felis pulvinar, tempus lectus at, viverra arcu. Sed facilisis convallis enim, sit amet gravida ex sodales aliquet. Aliquam fringilla gravida velit vitae dapibus. Duis fermentum a metus eget semper. Cras a felis ultrices nisl bibendum interdum eu sed leo. Sed eu placerat dolor. Morbi vel varius tellus. Integer a lacus tempor, sagittis ligula et, venenatis sem. Integer luctus sapien ac tortor facilisis mollis. Proin mattis eros iaculis justo varius, nec imperdiet nibh mattis. Nulla placerat justo tortor, a posuere sapien rutrum vel. Curabitur vel porta eros. Nam ligula nibh, gravida ut ante vel, accumsan ultricies elit. Proin mollis neque sed purus ultrices, quis egestas magna elementum. Nullam ac pulvinar magna. Etiam quis risus egestas massa rhoncus accumsan id sed metus. Curabitur pulvinar, tellus quis aliquet vehicula, arcu felis viverra felis, eget euismod nulla eros quis nisi. Nulla lorem est, dictum quis erat et, lacinia tincidunt lectus. Fusce placerat maximus ipsum, vitae fringilla nibh lacinia eget. Phasellus leo lacus, pretium vitae nulla in, ornare facilisis turpis. Praesent ut suscipit magna.";
  char* file3_text =  "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut in massa a augue fringilla condimentum rhoncus nec augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur vitae efficitur nibh. In commodo mauris posuere ipsum ullamcorper rutrum. Cras non nulla in justo imperdiet placerat. Mauris vehicula ultricies elementum. Proin suscipit vitae nisl eget pretium. Aenean in lacinia arcu. Phasellus nec ex eget dui accumsan consectetur a in nisi. Suspendisse lobortis ante ut euismod pellentesque. Nulla facilisi. Ut et euismod libero. Cras porta tempor diam et tincidunt. Donec et nibh aliquet, ultricies lorem ut, lacinia mi. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed nibh odio, sodales a nunc vel, vestibulum dictum dui. Sed faucibus mollis elit, eget luctus odio posuere tincidunt. Duis non elit lectus. Etiam eget lobortis lorem. Donec feugiat, mi sed blandit laoreet, leo orci laoreet nisi, non viverra diam sapien porttitor dolor. Donec porttitor orci at risus ullamcorper ornare. Fusce faucibus faucibus sapien vel porttitor. Vivamus urna mi, laoreet sagittis dignissim vel, imperdiet at turpis. Duis quis purus condimentum, ornare tellus sed, vestibulum sem. Aliquam leo risus, aliquet ac lectus malesuada, laoreet consectetur massa. Ut ante turpis, tempor vel justo ut, volutpat imperdiet felis. Cras non felis pulvinar, tempus lectus at, viverra arcu. Sed facilisis convallis enim, sit amet gravida ex sodales aliquet. Aliquam fringilla gravida velit vitae dapibus. Duis fermentum a metus eget semper. Cras a felis ultrices nisl bibendum interdum eu sed leo. Sed eu placerat dolor. Morbi vel varius tellus. Integer a lacus tempor, sagittis ligula et, venenatis sem. Integer luctus sapien ac tortor facilisis mollis. Proin mattis eros iaculis justo varius, nec imperdiet nibh mattis. Nulla placerat justo tortor, a posuere sapien rutrum vel. Curabitur vel porta eros. Nam ligula nibh, gravida ut ante vel, accumsan ultricies elit. Proin mollis neque sed purus ultrices, quis egestas magna elementum. Nullam ac pulvinar magna. Etiam quis risus egestas massa rhoncus accumsan id sed metus. Curabitur pulvinar, tellus quis aliquet vehicula, arcu felis viverra felis, eget euismod nulla eros quis nisi. Nulla lorem est, dictum quis erat et, lacinia tincidunt lectus. Fusce placerat maximus ipsum, vitae fringilla nibh lacinia eget. Phasellus leo lacus, pretium vitae nulla in, ornare facilisis turpis. Praesent ut suscipit magna. Phasellus venenatis arcu ipsum, eu venenatis quam egestas quis. Vivamus sit amet metus leo. Curabitur vitae est ut lorem mattis iaculis. Integer non ante quis nisi interdum commodo ac sed augue. Curabitur at venenatis ex. Mauris nec posuere lectus. Mauris condimentum velit sed velit bibendum pretium. Nunc cursus feugiat erat ac laoreet. Nam dolor purus, pharetra ut metus in, porttitor ultrices odio. Morbi quam turpis, placerat eget lacus ut, aliquam rutrum libero. Suspendisse potenti. Aenean ultricies ex ex, id porttitor augue dignissim in. Quisque id auctor ipsum. Morbi pellentesque diam ac egestas malesuada. Proin nec nibh eu massa imperdiet tincidunt. Nunc sollicitudin at est sit amet fringilla. Pellentesque faucibus neque quis enim laoreet, at bibendum tortor dignissim. Proin eget nulla sit amet velit luctus blandit a et felis. Pellentesque tortor sem, pretium non sagittis quis, blandit nec ligula. Nunc in pharetra ligula, a molestie libero. Etiam blandit id sapien non pretium. Praesent pharetra neque vitae dapibus efficitur. Nulla posuere eros ipsum, at consequat nunc faucibus et. Pellentesque eu lectus justo. Phasellus pulvinar, nibh vitae rhoncus luctus, urna ligula elementum ligula, at tincidunt nisi nisi a arcu. Curabitur vitae aliquet elit. Nunc pellentesque turpis ipsum, vel iaculis justo pulvinar eu. Pellentesque in nunc sit amet ante pulvinar sodales et nec tellus. Mauris molestie rutrum velit, porttitor semper nisi vestibulum ac. Maecenas eget sagittis justo. Mauris molestie, nisi at ullamcorper scelerisque, est sapien consectetur nulla, vitae mollis ex libero quis lorem. Curabitur id erat id mi posuere volutpat non vel turpis. Cras a blandit tortor. Nulla et justo eget ligula ultricies egestas in et ex. Suspendisse imperdiet bibendum purus. Nam malesuada congue nunc, sit amet mattis urna sollicitudin id. Donec mattis magna elit, et tempor massa pellentesque ac. Vestibulum eu dolor ac eros sollicitudin ultricies. Aliquam in ante id est pretium maximus. Nunc neque nibh, venenatis a tristique sed, vehicula et augue. In nec iaculis mauris. Proin arcu mi, molestie in orci vitae, tempor semper elit. Duis in libero luctus, sodales erat eget, rhoncus justo. Aenean tristique, turpis aliquam gravida accumsan, lacus mi fermentum quam, ac accumsan odio quam a augue. Ut ac lobortis ipsum. In malesuada tortor vel neque convallis semper. Proin ultrices, elit vitae lobortis pretium, nulla neque ultrices mi, sit amet auctor est ipsum eget nunc. Donec in elit nisi. Pellentesque semper, libero a pellentesque faucibus metus.";

  // Test writing and reading less than 512 bytes of text to an empty file
  printf("Writing text to file1... ");
  write_str_to_file("/", "file1", file1_text_1);
  printf("Reading text from file1... ");
  read_file("/", "file1", buf);
  file1a_test = memcmp(buf, file1_text_1, strlen(file1_text_1));
  if (!file1a_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10*BLOCK_SIZE);

  // Test writing and reading less than 512 bytes of text to a file with existing data
  printf("Writing more text to file1... ");
  write_str_to_file("/", "file1", file1_text_2);
  printf("Reading text from file1... ");
  read_file("/", "file1", buf);
  file1b_test = memcmp(buf+BLOCK_SIZE, file1_text_2, strlen(file1_text_2));
  if (!file1b_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10*BLOCK_SIZE);

  // Test writing and reading ~2560 bytes of text (5 blocks) to an empty file
  printf("Writing text to file2... ");
  write_str_to_file("/", "file2", file2_text);
  printf("Reading text from file2... ");
  read_file("/", "file2", buf);
  file2_test = memcmp(buf, file2_text, strlen(file2_text));
  if (!file2_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10*BLOCK_SIZE);

  // Test writing and reading ~5120 bytes of text (10 blocks) to an empty file
  printf("Writing text to file3... ");
  write_str_to_file("/", "file3", file3_text);
  printf("Reading text from file3... ");
  read_file("/", "file3", buf);
  file3_test = memcmp(buf, file3_text, strlen(file3_text));
  if (!file3_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10*BLOCK_SIZE);

  free(buf);

  if (file1a_test == 0 && file1b_test == 0 && file2_test == 0 && file3_test == 0) {
    printf("\nGOAL 2a: PASS\n\n");
    return 1;
  } else {
    printf("\nGOAL 2a: FAIL\n");
    if (file1a_test != 0)
      printf("Writing to and reading file 1 (initial) was unsuccessful.\n");
    if (file1b_test != 0)
      printf("Writing to and reading file 1 (after append) unsuccessful.\n");
    if (file2_test != 0)
      printf("Writing to and reading file 2 was unsuccessful.\n");
    if (file3_test != 0)
      printf("Writing to and reading file 3 was unsuccessful.\n");
    printf("\n");
    return 0;
  }
}

// Goal 2b: Creating subdirectories
// ====================================================================
// Tests the creation of several subdirectories.
int test_goal2b() {
  printf("TESTING GOAL 2b: Creation of subdirectories\n"
         "===========================================\n");

  InitLLFS();

  char *root_path = "/";
  DirectoryEntry root[DIRECTORY_CAPACITY];

  char *dir1_parent = root_path;
  char *dir1_name = "ycoady";
  char *dir1_path = "/ycoady";
  DirectoryEntry dir1[DIRECTORY_CAPACITY];

  char *dir2_parent = dir1_path;
  char *dir2_name = "teaching";
  char *dir2_path = "/ycoady/teaching";
  DirectoryEntry dir2[DIRECTORY_CAPACITY];

  char *dir34_parent = dir2_path;
  char *dir3_name = "spring2020";
  char *dir4_name = "summer2020";
  char *dir3_path = "/ycoady/teaching/spring2020";
  char *dir4_path = "/ycoady/teaching/summer2020";
  DirectoryEntry dir3[DIRECTORY_CAPACITY];
  DirectoryEntry dir4[DIRECTORY_CAPACITY];

  char *dir5_parent = dir3_path;
  char *dir5_name = "csc360";
  char *dir5_path = "/ycoady/teaching/spring2020/csc360";
  DirectoryEntry dir5[DIRECTORY_CAPACITY];

  char *dir6_parent = dir4_path;
  char *dir6_name = "csc462";
  char *dir6_path = "/ycoady/teaching/summer2020/csc462";
  DirectoryEntry dir6[DIRECTORY_CAPACITY];

  char *dir78_parent = dir5_path;
  char *dir7_name = "assignments";
  char *dir8_name = "exams";
  char *dir7_path = "/ycoady/teaching/spring2020/csc360/assignments";
  char *dir8_path = "/ycoady/teaching/spring2020/csc360/exams";
  DirectoryEntry dir7[DIRECTORY_CAPACITY];
  DirectoryEntry dir8[DIRECTORY_CAPACITY];

  char *dir9_parent = dir6_path;
  char *dir9_name = "assignments";
  char *dir9_path = "/ycoady/teaching/summer2020/csc462/assignments";
  DirectoryEntry dir9[DIRECTORY_CAPACITY];

  printf("Creating directories, subdirectories...\n");
  execute_mkdir(dir1_parent, dir1_name);
  execute_mkdir(dir2_parent, dir2_name);
  execute_mkdir(dir34_parent, dir3_name);
  execute_mkdir(dir34_parent, dir4_name);
  execute_mkdir(dir5_parent, dir5_name);
  execute_mkdir(dir6_parent, dir6_name);
  execute_mkdir(dir78_parent, dir7_name);
  execute_mkdir(dir78_parent, dir8_name);
  execute_mkdir(dir9_parent, dir9_name);

  int root_inode_index = get_leaf_dir_inode_index(root_path);
  int dir1_inode_index = get_leaf_dir_inode_index(dir1_path);
  int dir2_inode_index = get_leaf_dir_inode_index(dir2_path);
  int dir3_inode_index = get_leaf_dir_inode_index(dir3_path);
  int dir4_inode_index = get_leaf_dir_inode_index(dir4_path);
  int dir5_inode_index = get_leaf_dir_inode_index(dir5_path);
  int dir6_inode_index = get_leaf_dir_inode_index(dir6_path);
  int dir7_inode_index = get_leaf_dir_inode_index(dir7_path);
  int dir8_inode_index = get_leaf_dir_inode_index(dir8_path);
  int dir9_inode_index = get_leaf_dir_inode_index(dir9_path);

  get_directory_entries_from_inode_index(root_inode_index, root);
  get_directory_entries_from_inode_index(dir1_inode_index, dir1);
  get_directory_entries_from_inode_index(dir2_inode_index, dir2);
  get_directory_entries_from_inode_index(dir3_inode_index, dir3);
  get_directory_entries_from_inode_index(dir4_inode_index, dir4);
  get_directory_entries_from_inode_index(dir5_inode_index, dir5);
  get_directory_entries_from_inode_index(dir6_inode_index, dir6);
  get_directory_entries_from_inode_index(dir7_inode_index, dir7);
  get_directory_entries_from_inode_index(dir8_inode_index, dir8);
  get_directory_entries_from_inode_index(dir9_inode_index, dir9);

  /* We have
   * root = /
   * dir1 = /ycoady
   * dir2 = /ycoady/teaching
   * dir3 = /ycoady/teaching/spring2020
   * dir4 = /ycoady/teaching/summer2020
   * dir5 = /ycoady/teaching/spring2020/csc360
   * dir6 = /ycoady/teaching/summer2020/csc462
   * dir7 = /ycoady/teaching/spring2020/csc360/assignments
   * dir8 = /ycoady/teaching/spring2020/csc360/exams
   * dir9 = /ycoady/teaching/spring2020/csc462/assignments
  */

  printf("Checking all the expected directory entries are present...\n");
  int dir1_present = strcmp(root[0].file_name, dir1_name);
  int dir2_present = strcmp(dir1[0].file_name, dir2_name);
  int dir3_present = strcmp(dir2[0].file_name, dir3_name);
  int dir4_present = strcmp(dir2[1].file_name, dir4_name);
  int dir5_present = strcmp(dir3[0].file_name, dir5_name);
  int dir6_present = strcmp(dir4[0].file_name, dir6_name);
  int dir7_present = strcmp(dir5[0].file_name, dir7_name);
  int dir8_present = strcmp(dir5[1].file_name, dir8_name);
  int dir9_present = strcmp(dir6[0].file_name, dir9_name);

  if (!dir1_present && !dir2_present && !dir3_present && !dir4_present && !dir5_present && !dir6_present
   && !dir7_present && !dir8_present && !dir9_present) {
    printf("\nGOAL 2b: PASS\n\n");
    return 1;
  } else {
    printf("\nGOAL 2b: FAIL\n");
    if (dir1_present != 0) printf("Directory %s not present in %s.\n", dir1_name, dir1_parent);
    if (dir2_present != 0) printf("Directory %s not present in %s.\n", dir2_name, dir2_parent);
    if (dir3_present != 0) printf("Directory %s not present in %s.\n", dir3_name, dir34_parent);
    if (dir4_present != 0) printf("Directory %s not present in %s.\n", dir4_name, dir34_parent);
    if (dir5_present != 0) printf("Directory %s not present in %s.\n", dir5_name, dir5_parent);
    if (dir6_present != 0) printf("Directory %s not present in %s.\n", dir6_name, dir6_parent);
    if (dir7_present != 0) printf("Directory %s not present in %s.\n", dir7_name, dir78_parent);
    if (dir8_present != 0) printf("Directory %s not present in %s.\n", dir8_name, dir78_parent);
    if (dir9_present != 0) printf("Directory %s not present in %s.\n", dir9_name, dir9_parent);
    printf("\n");
    return 0;
  }
}

// Goal 2c: Reading and writing of file in any directory
// =====================================================================
// Creates 7 test files in 3 different directories
// Writes 1 block of text to file1, then appends another
// Writes 5 blocks of text to file2, file5
// Writes 10 blocks of text to file3, file6
// Writes 1 block of text to file4, file7
// Reads and compares the contents of each file with the text that was written to them
int test_goal2c() {
  printf("TESTING GOAL 2c: Reading and writing files in any directory\n"
         "===========================================================\n");

  InitLLFS();

  // Test flags
  int file1a_test = -1;
  int file1b_test = -1;
  int file2_test = -1;
  int file3_test = -1;
  int file4_test = -1;
  int file5_test = -1;
  int file6_test = -1;
  int file7_test = -1;
  int dir1_present = -1;
  int dir2_present = -1;
  int dir3_present = -1;

  char *buf = calloc(10 * BLOCK_SIZE, 1);

  char *root_path = "/";
  DirectoryEntry root[DIRECTORY_CAPACITY];

  char *dir1_parent = root_path;
  char *dir1_name = "foo";
  char *dir1_path = "/foo";
  DirectoryEntry dir1[DIRECTORY_CAPACITY];

  char *dir2_parent = dir1_path;
  char *dir2_name = "bar";
  char *dir2_path = "/foo/bar";
  DirectoryEntry dir2[DIRECTORY_CAPACITY];

  char *dir3_parent = dir2_path;
  char *dir3_name = "baz";
  char *dir3_path = "/foo/bar/baz";
  DirectoryEntry dir3[DIRECTORY_CAPACITY];

  char *file1_text_1 = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut in massa a augue fringilla condimentum rhoncus nec augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur vitae efficitur nibh. In commodo mauris posuere ipsum ullamcorper rutrum. Cras non nulla in justo imperdiet placerat. Mauris vehicula ultricies elementum. Proin suscipit vitae nisl eget pretium. Aenean in lacinia arcu. Phasellus nec ex eget dui accumsan consectetur a in nisi. Suspendisse";
  char *file1_text_2 = " lobortis ante ut euismod pellentesque. Nulla facilisi. Ut et euismod libero. Cras porta tempor diam et tincidunt. Donec et nibh aliquet, ultricies lorem ut, lacinia mi. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed nibh odio, sodales a nunc vel, vestibulum dictum dui. Sed faucibus mollis elit, eget luctus odio posuere tincidunt. Duis non elit lectus. Etiam eget lobortis lorem.";
  char *file2_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut in massa a augue fringilla condimentum rhoncus nec augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur vitae efficitur nibh. In commodo mauris posuere ipsum ullamcorper rutrum. Cras non nulla in justo imperdiet placerat. Mauris vehicula ultricies elementum. Proin suscipit vitae nisl eget pretium. Aenean in lacinia arcu. Phasellus nec ex eget dui accumsan consectetur a in nisi. Suspendisse lobortis ante ut euismod pellentesque. Nulla facilisi. Ut et euismod libero. Cras porta tempor diam et tincidunt. Donec et nibh aliquet, ultricies lorem ut, lacinia mi. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed nibh odio, sodales a nunc vel, vestibulum dictum dui. Sed faucibus mollis elit, eget luctus odio posuere tincidunt. Duis non elit lectus. Etiam eget lobortis lorem. Donec feugiat, mi sed blandit laoreet, leo orci laoreet nisi, non viverra diam sapien porttitor dolor. Donec porttitor orci at risus ullamcorper ornare. Fusce faucibus faucibus sapien vel porttitor. Vivamus urna mi, laoreet sagittis dignissim vel, imperdiet at turpis. Duis quis purus condimentum, ornare tellus sed, vestibulum sem. Aliquam leo risus, aliquet ac lectus malesuada, laoreet consectetur massa. Ut ante turpis, tempor vel justo ut, volutpat imperdiet felis. Cras non felis pulvinar, tempus lectus at, viverra arcu. Sed facilisis convallis enim, sit amet gravida ex sodales aliquet. Aliquam fringilla gravida velit vitae dapibus. Duis fermentum a metus eget semper. Cras a felis ultrices nisl bibendum interdum eu sed leo. Sed eu placerat dolor. Morbi vel varius tellus. Integer a lacus tempor, sagittis ligula et, venenatis sem. Integer luctus sapien ac tortor facilisis mollis. Proin mattis eros iaculis justo varius, nec imperdiet nibh mattis. Nulla placerat justo tortor, a posuere sapien rutrum vel. Curabitur vel porta eros. Nam ligula nibh, gravida ut ante vel, accumsan ultricies elit. Proin mollis neque sed purus ultrices, quis egestas magna elementum. Nullam ac pulvinar magna. Etiam quis risus egestas massa rhoncus accumsan id sed metus. Curabitur pulvinar, tellus quis aliquet vehicula, arcu felis viverra felis, eget euismod nulla eros quis nisi. Nulla lorem est, dictum quis erat et, lacinia tincidunt lectus. Fusce placerat maximus ipsum, vitae fringilla nibh lacinia eget. Phasellus leo lacus, pretium vitae nulla in, ornare facilisis turpis. Praesent ut suscipit magna.";
  char *file3_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut in massa a augue fringilla condimentum rhoncus nec augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur vitae efficitur nibh. In commodo mauris posuere ipsum ullamcorper rutrum. Cras non nulla in justo imperdiet placerat. Mauris vehicula ultricies elementum. Proin suscipit vitae nisl eget pretium. Aenean in lacinia arcu. Phasellus nec ex eget dui accumsan consectetur a in nisi. Suspendisse lobortis ante ut euismod pellentesque. Nulla facilisi. Ut et euismod libero. Cras porta tempor diam et tincidunt. Donec et nibh aliquet, ultricies lorem ut, lacinia mi. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed nibh odio, sodales a nunc vel, vestibulum dictum dui. Sed faucibus mollis elit, eget luctus odio posuere tincidunt. Duis non elit lectus. Etiam eget lobortis lorem. Donec feugiat, mi sed blandit laoreet, leo orci laoreet nisi, non viverra diam sapien porttitor dolor. Donec porttitor orci at risus ullamcorper ornare. Fusce faucibus faucibus sapien vel porttitor. Vivamus urna mi, laoreet sagittis dignissim vel, imperdiet at turpis. Duis quis purus condimentum, ornare tellus sed, vestibulum sem. Aliquam leo risus, aliquet ac lectus malesuada, laoreet consectetur massa. Ut ante turpis, tempor vel justo ut, volutpat imperdiet felis. Cras non felis pulvinar, tempus lectus at, viverra arcu. Sed facilisis convallis enim, sit amet gravida ex sodales aliquet. Aliquam fringilla gravida velit vitae dapibus. Duis fermentum a metus eget semper. Cras a felis ultrices nisl bibendum interdum eu sed leo. Sed eu placerat dolor. Morbi vel varius tellus. Integer a lacus tempor, sagittis ligula et, venenatis sem. Integer luctus sapien ac tortor facilisis mollis. Proin mattis eros iaculis justo varius, nec imperdiet nibh mattis. Nulla placerat justo tortor, a posuere sapien rutrum vel. Curabitur vel porta eros. Nam ligula nibh, gravida ut ante vel, accumsan ultricies elit. Proin mollis neque sed purus ultrices, quis egestas magna elementum. Nullam ac pulvinar magna. Etiam quis risus egestas massa rhoncus accumsan id sed metus. Curabitur pulvinar, tellus quis aliquet vehicula, arcu felis viverra felis, eget euismod nulla eros quis nisi. Nulla lorem est, dictum quis erat et, lacinia tincidunt lectus. Fusce placerat maximus ipsum, vitae fringilla nibh lacinia eget. Phasellus leo lacus, pretium vitae nulla in, ornare facilisis turpis. Praesent ut suscipit magna. Phasellus venenatis arcu ipsum, eu venenatis quam egestas quis. Vivamus sit amet metus leo. Curabitur vitae est ut lorem mattis iaculis. Integer non ante quis nisi interdum commodo ac sed augue. Curabitur at venenatis ex. Mauris nec posuere lectus. Mauris condimentum velit sed velit bibendum pretium. Nunc cursus feugiat erat ac laoreet. Nam dolor purus, pharetra ut metus in, porttitor ultrices odio. Morbi quam turpis, placerat eget lacus ut, aliquam rutrum libero. Suspendisse potenti. Aenean ultricies ex ex, id porttitor augue dignissim in. Quisque id auctor ipsum. Morbi pellentesque diam ac egestas malesuada. Proin nec nibh eu massa imperdiet tincidunt. Nunc sollicitudin at est sit amet fringilla. Pellentesque faucibus neque quis enim laoreet, at bibendum tortor dignissim. Proin eget nulla sit amet velit luctus blandit a et felis. Pellentesque tortor sem, pretium non sagittis quis, blandit nec ligula. Nunc in pharetra ligula, a molestie libero. Etiam blandit id sapien non pretium. Praesent pharetra neque vitae dapibus efficitur. Nulla posuere eros ipsum, at consequat nunc faucibus et. Pellentesque eu lectus justo. Phasellus pulvinar, nibh vitae rhoncus luctus, urna ligula elementum ligula, at tincidunt nisi nisi a arcu. Curabitur vitae aliquet elit. Nunc pellentesque turpis ipsum, vel iaculis justo pulvinar eu. Pellentesque in nunc sit amet ante pulvinar sodales et nec tellus. Mauris molestie rutrum velit, porttitor semper nisi vestibulum ac. Maecenas eget sagittis justo. Mauris molestie, nisi at ullamcorper scelerisque, est sapien consectetur nulla, vitae mollis ex libero quis lorem. Curabitur id erat id mi posuere volutpat non vel turpis. Cras a blandit tortor. Nulla et justo eget ligula ultricies egestas in et ex. Suspendisse imperdiet bibendum purus. Nam malesuada congue nunc, sit amet mattis urna sollicitudin id. Donec mattis magna elit, et tempor massa pellentesque ac. Vestibulum eu dolor ac eros sollicitudin ultricies. Aliquam in ante id est pretium maximus. Nunc neque nibh, venenatis a tristique sed, vehicula et augue. In nec iaculis mauris. Proin arcu mi, molestie in orci vitae, tempor semper elit. Duis in libero luctus, sodales erat eget, rhoncus justo. Aenean tristique, turpis aliquam gravida accumsan, lacus mi fermentum quam, ac accumsan odio quam a augue. Ut ac lobortis ipsum. In malesuada tortor vel neque convallis semper. Proin ultrices, elit vitae lobortis pretium, nulla neque ultrices mi, sit amet auctor est ipsum eget nunc. Donec in elit nisi. Pellentesque semper, libero a pellentesque faucibus metus.";

  char *file4_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut in massa a augue fringilla condimentum rhoncus nec augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur vitae efficitur nibh. In commodo mauris posuere ipsum ullamcorper rutrum. Cras non nulla in justo imperdiet placerat. Mauris vehicula ultricies elementum. Proin suscipit vitae nisl eget pretium. Aenean in lacinia arcu. Phasellus nec ex eget dui accumsan consectetur a in nisi. Suspendisse";
  char *file5_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut in massa a augue fringilla condimentum rhoncus nec augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur vitae efficitur nibh. In commodo mauris posuere ipsum ullamcorper rutrum. Cras non nulla in justo imperdiet placerat. Mauris vehicula ultricies elementum. Proin suscipit vitae nisl eget pretium. Aenean in lacinia arcu. Phasellus nec ex eget dui accumsan consectetur a in nisi. Suspendisse lobortis ante ut euismod pellentesque. Nulla facilisi. Ut et euismod libero. Cras porta tempor diam et tincidunt. Donec et nibh aliquet, ultricies lorem ut, lacinia mi. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed nibh odio, sodales a nunc vel, vestibulum dictum dui. Sed faucibus mollis elit, eget luctus odio posuere tincidunt. Duis non elit lectus. Etiam eget lobortis lorem. Donec feugiat, mi sed blandit laoreet, leo orci laoreet nisi, non viverra diam sapien porttitor dolor. Donec porttitor orci at risus ullamcorper ornare. Fusce faucibus faucibus sapien vel porttitor. Vivamus urna mi, laoreet sagittis dignissim vel, imperdiet at turpis. Duis quis purus condimentum, ornare tellus sed, vestibulum sem. Aliquam leo risus, aliquet ac lectus malesuada, laoreet consectetur massa. Ut ante turpis, tempor vel justo ut, volutpat imperdiet felis. Cras non felis pulvinar, tempus lectus at, viverra arcu. Sed facilisis convallis enim, sit amet gravida ex sodales aliquet. Aliquam fringilla gravida velit vitae dapibus. Duis fermentum a metus eget semper. Cras a felis ultrices nisl bibendum interdum eu sed leo. Sed eu placerat dolor. Morbi vel varius tellus. Integer a lacus tempor, sagittis ligula et, venenatis sem. Integer luctus sapien ac tortor facilisis mollis. Proin mattis eros iaculis justo varius, nec imperdiet nibh mattis. Nulla placerat justo tortor, a posuere sapien rutrum vel. Curabitur vel porta eros. Nam ligula nibh, gravida ut ante vel, accumsan ultricies elit. Proin mollis neque sed purus ultrices, quis egestas magna elementum. Nullam ac pulvinar magna. Etiam quis risus egestas massa rhoncus accumsan id sed metus. Curabitur pulvinar, tellus quis aliquet vehicula, arcu felis viverra felis, eget euismod nulla eros quis nisi. Nulla lorem est, dictum quis erat et, lacinia tincidunt lectus. Fusce placerat maximus ipsum, vitae fringilla nibh lacinia eget. Phasellus leo lacus, pretium vitae nulla in, ornare facilisis turpis. Praesent ut suscipit magna.";
  char *file6_text = "Lorem ipsum dolor sit amet, consectetur adipiscing elit. Ut in massa a augue fringilla condimentum rhoncus nec augue. Pellentesque habitant morbi tristique senectus et netus et malesuada fames ac turpis egestas. Curabitur vitae efficitur nibh. In commodo mauris posuere ipsum ullamcorper rutrum. Cras non nulla in justo imperdiet placerat. Mauris vehicula ultricies elementum. Proin suscipit vitae nisl eget pretium. Aenean in lacinia arcu. Phasellus nec ex eget dui accumsan consectetur a in nisi. Suspendisse lobortis ante ut euismod pellentesque. Nulla facilisi. Ut et euismod libero. Cras porta tempor diam et tincidunt. Donec et nibh aliquet, ultricies lorem ut, lacinia mi. Class aptent taciti sociosqu ad litora torquent per conubia nostra, per inceptos himenaeos. Sed nibh odio, sodales a nunc vel, vestibulum dictum dui. Sed faucibus mollis elit, eget luctus odio posuere tincidunt. Duis non elit lectus. Etiam eget lobortis lorem. Donec feugiat, mi sed blandit laoreet, leo orci laoreet nisi, non viverra diam sapien porttitor dolor. Donec porttitor orci at risus ullamcorper ornare. Fusce faucibus faucibus sapien vel porttitor. Vivamus urna mi, laoreet sagittis dignissim vel, imperdiet at turpis. Duis quis purus condimentum, ornare tellus sed, vestibulum sem. Aliquam leo risus, aliquet ac lectus malesuada, laoreet consectetur massa. Ut ante turpis, tempor vel justo ut, volutpat imperdiet felis. Cras non felis pulvinar, tempus lectus at, viverra arcu. Sed facilisis convallis enim, sit amet gravida ex sodales aliquet. Aliquam fringilla gravida velit vitae dapibus. Duis fermentum a metus eget semper. Cras a felis ultrices nisl bibendum interdum eu sed leo. Sed eu placerat dolor. Morbi vel varius tellus. Integer a lacus tempor, sagittis ligula et, venenatis sem. Integer luctus sapien ac tortor facilisis mollis. Proin mattis eros iaculis justo varius, nec imperdiet nibh mattis. Nulla placerat justo tortor, a posuere sapien rutrum vel. Curabitur vel porta eros. Nam ligula nibh, gravida ut ante vel, accumsan ultricies elit. Proin mollis neque sed purus ultrices, quis egestas magna elementum. Nullam ac pulvinar magna. Etiam quis risus egestas massa rhoncus accumsan id sed metus. Curabitur pulvinar, tellus quis aliquet vehicula, arcu felis viverra felis, eget euismod nulla eros quis nisi. Nulla lorem est, dictum quis erat et, lacinia tincidunt lectus. Fusce placerat maximus ipsum, vitae fringilla nibh lacinia eget. Phasellus leo lacus, pretium vitae nulla in, ornare facilisis turpis. Praesent ut suscipit magna. Phasellus venenatis arcu ipsum, eu venenatis quam egestas quis. Vivamus sit amet metus leo. Curabitur vitae est ut lorem mattis iaculis. Integer non ante quis nisi interdum commodo ac sed augue. Curabitur at venenatis ex. Mauris nec posuere lectus. Mauris condimentum velit sed velit bibendum pretium. Nunc cursus feugiat erat ac laoreet. Nam dolor purus, pharetra ut metus in, porttitor ultrices odio. Morbi quam turpis, placerat eget lacus ut, aliquam rutrum libero. Suspendisse potenti. Aenean ultricies ex ex, id porttitor augue dignissim in. Quisque id auctor ipsum. Morbi pellentesque diam ac egestas malesuada. Proin nec nibh eu massa imperdiet tincidunt. Nunc sollicitudin at est sit amet fringilla. Pellentesque faucibus neque quis enim laoreet, at bibendum tortor dignissim. Proin eget nulla sit amet velit luctus blandit a et felis. Pellentesque tortor sem, pretium non sagittis quis, blandit nec ligula. Nunc in pharetra ligula, a molestie libero. Etiam blandit id sapien non pretium. Praesent pharetra neque vitae dapibus efficitur. Nulla posuere eros ipsum, at consequat nunc faucibus et. Pellentesque eu lectus justo. Phasellus pulvinar, nibh vitae rhoncus luctus, urna ligula elementum ligula, at tincidunt nisi nisi a arcu. Curabitur vitae aliquet elit. Nunc pellentesque turpis ipsum, vel iaculis justo pulvinar eu. Pellentesque in nunc sit amet ante pulvinar sodales et nec tellus. Mauris molestie rutrum velit, porttitor semper nisi vestibulum ac. Maecenas eget sagittis justo. Mauris molestie, nisi at ullamcorper scelerisque, est sapien consectetur nulla, vitae mollis ex libero quis lorem. Curabitur id erat id mi posuere volutpat non vel turpis. Cras a blandit tortor. Nulla et justo eget ligula ultricies egestas in et ex. Suspendisse imperdiet bibendum purus. Nam malesuada congue nunc, sit amet mattis urna sollicitudin id. Donec mattis magna elit, et tempor massa pellentesque ac. Vestibulum eu dolor ac eros sollicitudin ultricies. Aliquam in ante id est pretium maximus. Nunc neque nibh, venenatis a tristique sed, vehicula et augue. In nec iaculis mauris. Proin arcu mi, molestie in orci vitae, tempor semper elit. Duis in libero luctus, sodales erat eget, rhoncus justo. Aenean tristique, turpis aliquam gravida accumsan, lacus mi fermentum quam, ac accumsan odio quam a augue. Ut ac lobortis ipsum. In malesuada tortor vel neque convallis semper. Proin ultrices, elit vitae lobortis pretium, nulla neque ultrices mi, sit amet auctor est ipsum eget nunc. Donec in elit nisi. Pellentesque semper, libero a pellentesque faucibus metus.";
  char *file7_text = "this is a small amount of text";

  char *file1_parent = root_path;
  char *file1_name = "test1.txt";

  char *file23_parent = dir1_path;
  char *file2_name = "test2.txt";
  char *file3_name = "test3.txt";

  char *file45_parent = dir2_path;
  char *file4_name = "test4.txt";
  char *file5_name = "test5.txt";

  char *file67_parent = dir3_path;
  char *file6_name = "test6.txt";
  char *file7_name = "test7.txt";

  printf("Creating directories, subdirectories...\n");
  execute_mkdir(dir1_parent, dir1_name);
  execute_mkdir(dir2_parent, dir2_name);
  execute_mkdir(dir3_parent, dir3_name);

  printf("Creating files...\n");
  execute_touch(file1_parent, file1_name);
  execute_touch(file23_parent, file2_name);
  execute_touch(file23_parent, file3_name);
  execute_touch(file45_parent, file4_name);
  execute_touch(file45_parent, file5_name);
  execute_touch(file67_parent, file6_name);
  execute_touch(file67_parent, file7_name);

  // Test writing and reading less than 512 bytes of text to an empty file
  printf("Writing text to file1... ");
  write_str_to_file(file1_parent, file1_name, file1_text_1);
  printf("Reading text from file1... ");
  read_file(file1_parent, file1_name, buf);
  file1a_test = memcmp(buf, file1_text_1, strlen(file1_text_1));
  if (!file1a_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10 * BLOCK_SIZE);

  // Test writing and reading less than 512 bytes of text to a file with existing data
  printf("Writing more text to file1... ");
  write_str_to_file(file1_parent, file1_name, file1_text_2);
  printf("Reading text from file1... ");
  read_file(file1_parent, file1_name, buf);
  file1b_test = memcmp(buf + BLOCK_SIZE, file1_text_2, strlen(file1_text_2));
  if (!file1b_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10 * BLOCK_SIZE);

  // Test writing and reading ~2560 bytes of text (5 blocks) to an empty file
  printf("Writing text to file2... ");
  write_str_to_file(file23_parent, file2_name, file2_text);
  printf("Reading text from file2... ");
  read_file(file23_parent, file2_name, buf);
  file2_test = memcmp(buf, file2_text, strlen(file2_text));
  if (!file2_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10 * BLOCK_SIZE);

  // Test writing and reading ~5120 bytes of text (10 blocks) to an empty file
  printf("Writing text to file3... ");
  write_str_to_file(file23_parent, file3_name, file3_text);
  printf("Reading text from file3... ");
  read_file(file23_parent, file3_name, buf);
  file3_test = memcmp(buf, file3_text, strlen(file3_text));
  if (!file3_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10 * BLOCK_SIZE);

  // Test writing and reading less than 512 bytes of text to an empty file
  printf("Writing text to file4... ");
  write_str_to_file(file45_parent, file4_name, file4_text);
  printf("Reading text from file4... ");
  read_file(file1_parent, file1_name, buf);
  file4_test = memcmp(buf, file4_text, strlen(file4_text));
  if (!file4_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10 * BLOCK_SIZE);

  // Test writing and reading ~2560 bytes of text (5 blocks) to an empty file
  printf("Writing text to file5... ");
  write_str_to_file(file45_parent, file5_name, file5_text);
  printf("Reading text from file5... ");
  read_file(file45_parent, file5_name, buf);
  file5_test = memcmp(buf, file5_text, strlen(file5_text));
  if (!file5_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10 * BLOCK_SIZE);

  // Test writing and reading ~5120 bytes of text (10 blocks) to an empty file
  printf("Writing text to file6... ");
  write_str_to_file(file67_parent, file6_name, file6_text);
  printf("Reading text from file6... ");
  read_file(file67_parent, file6_name, buf);
  file6_test = memcmp(buf, file6_text, strlen(file6_text));
  if (!file6_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10 * BLOCK_SIZE);

  // Test writing and reading a small amount of text to an empty file
  printf("Writing text to file7... ");
  write_str_to_file(file67_parent, file7_name, file7_text);
  printf("Reading text from file7... ");
  read_file(file67_parent, file7_name, buf);
  file7_test = memcmp(buf, file7_text, strlen(file7_text));
  if (!file7_test) printf("OK\n");
  else printf("\n");
  memset(buf, 0, 10 * BLOCK_SIZE);

  int root_inode_index = get_leaf_dir_inode_index(root_path);
  int dir1_inode_index = get_leaf_dir_inode_index(dir1_path);
  int dir2_inode_index = get_leaf_dir_inode_index(dir2_path);
  int dir3_inode_index = get_leaf_dir_inode_index(dir3_path);

  get_directory_entries_from_inode_index(root_inode_index, root);
  get_directory_entries_from_inode_index(dir1_inode_index, dir1);
  get_directory_entries_from_inode_index(dir2_inode_index, dir2);
  get_directory_entries_from_inode_index(dir3_inode_index, dir3);

  printf("Checking all the expected directory entries are present...\n");
  dir1_present = strcmp(root[0].file_name, dir1_name);
  dir2_present = strcmp(dir1[0].file_name, dir2_name);
  dir3_present = strcmp(dir2[0].file_name, dir3_name);

  free(buf);

  if (!file1a_test && !file1b_test && !file3_test && !file4_test && !file5_test && !file6_test && !file7_test
      && !dir1_present && !dir2_present && !dir3_present) {
    printf("\nGOAL 2c: PASS\n\n");
    return 1;
  } else {
    printf("\nGOAL 2c: FAIL\n");
    if (file1a_test != 0) printf("Writing to and reading file 1 (initial) was unsuccessful.\n");
    if (file1b_test != 0) printf("Writing to and reading file 1 (after append) unsuccessful.\n");
    if (file2_test != 0) printf("Writing to and reading file 2 was unsuccessful.\n");
    if (file3_test != 0) printf("Writing to and reading file 3 was unsuccessful.\n");
    if (file4_test != 0) printf("Writing to and reading file 4 was unsuccessful.\n");
    if (file5_test != 0) printf("Writing to and reading file 5 was unsuccessful.\n");
    if (file6_test != 0) printf("Writing to and reading file 6 was unsuccessful.\n");
    if (file7_test != 0) printf("Writing to and reading file 7 was unsuccessful.\n");
    if (dir1_present != 0) printf("Directory %s not present in %s.\n", dir1_name, dir1_parent);
    if (dir2_present != 0) printf("Directory %s not present in %s.\n", dir2_name, dir2_parent);
    if (dir3_present != 0) printf("Directory %s not present in %s.\n", dir3_name, dir3_parent);
    printf("\n");
    return 0;
  }
}

// Goal 2d: Deletion of files and directories
// ===============================================
// Creates 7 test files in 3 different directories
// Tries deleting a non-empty directory (expected to error)
// Deletes some of the files, including all files in the third directory
// Deletes the third directory
int test_goal2d() {
  printf("TESTING GOAL 2d: Deletion of files and directories\n"
         "==================================================\n");

  InitLLFS();

  // Test flags
  int file1_before_deletion = 0;
  int file2_before_deletion = 0;
  int file3_before_deletion = 0;
  int file4_before_deletion = 0;
  int file5_before_deletion = 0;
  int file6_before_deletion = 0;
  int file7_before_deletion = 0;
  int dir1_before_deletion = 0;
  int dir2_before_deletion = 0;
  int dir3_before_deletion = 0;

  int file1_after_deletion = 0;
  int file2_after_deletion = 0;
  int file3_after_deletion = 0;
  int file4_after_deletion = 0;
  int file5_after_deletion = 0;
  int file6_after_deletion = 0;
  int file7_after_deletion = 0;
  int dir1_after_deletion = 0;
  int dir2_after_deletion = 0;
  int dir3_after_deletion = 0;

  int files_before_deletion = 0;
  int files_after_deletion = 0;
  int dirs_before_deletion = 0;
  int dirs_after_deletion = 0;
  int dir3_failed_deletion = 0;
  int non_empty_test = 0;

  char *root_path = "/";
  DirectoryEntry root_before[DIRECTORY_CAPACITY];
  DirectoryEntry root_after[DIRECTORY_CAPACITY];

  char *dir1_parent = root_path;
  char *dir1_name = "foo";
  char *dir1_path = "/foo";
  DirectoryEntry dir1_before[DIRECTORY_CAPACITY];
  DirectoryEntry dir1_after[DIRECTORY_CAPACITY];

  char *dir2_parent = dir1_path;
  char *dir2_name = "bar";
  char *dir2_path = "/foo/bar";
  DirectoryEntry dir2_before[DIRECTORY_CAPACITY];
  DirectoryEntry dir2_after[DIRECTORY_CAPACITY];
  DirectoryEntry dir2_after_dir3_deletion[DIRECTORY_CAPACITY];

  char *dir3_parent = dir2_path;
  char *dir3_name = "baz";
  char *dir3_path = "/foo/bar/baz";
  DirectoryEntry dir3_before[DIRECTORY_CAPACITY];
  DirectoryEntry dir3_after[DIRECTORY_CAPACITY];

  char *file1_parent = root_path;
  char *file1_name = "test1.txt";

  char *file23_parent = dir1_path;
  char *file2_name = "test2.txt";
  char *file3_name = "test3.txt";

  char *file45_parent = dir2_path;
  char *file4_name = "test4.txt";
  char *file5_name = "test5.txt";

  char *file67_parent = dir3_path;
  char *file6_name = "test6.txt";
  char *file7_name = "test7.txt";

  printf("Creating directories, subdirectories...\n");
  execute_mkdir(dir1_parent, dir1_name);
  execute_mkdir(dir2_parent, dir2_name);
  execute_mkdir(dir3_parent, dir3_name);

  printf("Creating files...\n");
  execute_touch(file1_parent, file1_name);
  execute_touch(file23_parent, file2_name);
  execute_touch(file23_parent, file3_name);
  execute_touch(file45_parent, file4_name);
  execute_touch(file45_parent, file5_name);
  execute_touch(file67_parent, file6_name);
  execute_touch(file67_parent, file7_name);

  int root_inode_index = get_leaf_dir_inode_index(root_path);
  int dir1_inode_index = get_leaf_dir_inode_index(dir1_path);
  int dir2_inode_index = get_leaf_dir_inode_index(dir2_path);
  int dir3_inode_index = get_leaf_dir_inode_index(dir3_path);

  get_directory_entries_from_inode_index(root_inode_index, root_before);
  get_directory_entries_from_inode_index(dir1_inode_index, dir1_before);
  get_directory_entries_from_inode_index(dir2_inode_index, dir2_before);
  get_directory_entries_from_inode_index(dir3_inode_index, dir3_before);

  printf("Checking all the expected directories are present... ");
  dir1_before_deletion = get_file_inode_index(dir1_name, root_before);
  dir2_before_deletion = get_file_inode_index(dir2_name, dir1_before);
  dir3_before_deletion = get_file_inode_index(dir3_name, dir2_before);
  if (dir1_before_deletion != NOT_FOUND && dir2_before_deletion != NOT_FOUND && dir3_before_deletion != NOT_FOUND) {
    dirs_before_deletion = 1;
    printf("OK\n");
  }
  printf("\n");

  printf("Checking all the expected files are present... ");
  file1_before_deletion = get_file_inode_index(file1_name, root_before);
  file2_before_deletion = get_file_inode_index(file2_name, dir1_before);
  file3_before_deletion = get_file_inode_index(file3_name, dir1_before);
  file4_before_deletion = get_file_inode_index(file4_name, dir2_before);
  file5_before_deletion = get_file_inode_index(file5_name, dir2_before);
  file6_before_deletion = get_file_inode_index(file6_name, dir3_before);
  file7_before_deletion = get_file_inode_index(file7_name, dir3_before);
  if (file1_before_deletion != NOT_FOUND && file2_before_deletion != NOT_FOUND && file3_before_deletion != NOT_FOUND
      && file4_before_deletion != NOT_FOUND && file5_before_deletion != NOT_FOUND && file6_before_deletion != NOT_FOUND
      && file7_before_deletion != NOT_FOUND) {
    files_before_deletion = 1;
    printf("OK\n");
  }
  printf("\n");

  printf("Attempting to delete non-empty directory /foo/bar/baz... ");
  execute_rm(dir3_parent, dir3_name);
  dir3_failed_deletion = get_file_inode_index(dir3_name, dir2_before);
  if (dir3_failed_deletion != NOT_FOUND) {
    non_empty_test = 1;
    printf("OK\n");
  }

  printf("Deleting files 1, 4, 5, 6, 7... ");
  execute_rm(file1_parent, file1_name);
  execute_rm(file45_parent, file4_name);
  execute_rm(file45_parent, file5_name);
  execute_rm(file67_parent, file6_name);
  execute_rm(file67_parent, file7_name);

  get_directory_entries_from_inode_index(root_inode_index, root_after);
  get_directory_entries_from_inode_index(dir1_inode_index, dir1_after);
  get_directory_entries_from_inode_index(dir2_inode_index, dir2_after);
  get_directory_entries_from_inode_index(dir3_inode_index, dir3_after);

  printf("Checking that the files were actually deleted... ");
  file1_after_deletion = get_file_inode_index(file1_name, root_after);
  file2_after_deletion = get_file_inode_index(file2_name, dir1_after);
  file3_after_deletion = get_file_inode_index(file3_name, dir1_after);
  file4_after_deletion = get_file_inode_index(file4_name, dir2_after);
  file5_after_deletion = get_file_inode_index(file5_name, dir2_after);
  file6_after_deletion = get_file_inode_index(file6_name, dir3_after);
  file7_after_deletion = get_file_inode_index(file7_name, dir3_after);
  if (file1_after_deletion == NOT_FOUND && file2_after_deletion != NOT_FOUND && file3_after_deletion != NOT_FOUND
      && file4_after_deletion == NOT_FOUND && file5_after_deletion == NOT_FOUND && file6_after_deletion == NOT_FOUND
      && file7_after_deletion == NOT_FOUND) {
    files_after_deletion = 1;
    printf("OK\n");
  }
  printf("\n");

  printf("Deleting now-empty directory /foo/bar/baz...\n");
  execute_rm(dir3_parent, dir3_name);

  get_directory_entries_from_inode_index(dir2_inode_index, dir2_after_dir3_deletion);
  dir1_after_deletion = get_file_inode_index(dir1_name, root_after);
  dir2_after_deletion = get_file_inode_index(dir2_name, dir1_after);
  dir3_after_deletion = get_file_inode_index(dir3_name, dir2_after_dir3_deletion);
  printf("Checking that /foo/bar/baz was deleted... ");
  if (dir1_after_deletion != NOT_FOUND && dir2_after_deletion != NOT_FOUND && dir3_after_deletion == NOT_FOUND) {
    dirs_after_deletion = 1;
    printf("OK\n");
  }

  if (files_before_deletion && dirs_before_deletion && files_after_deletion && dirs_after_deletion && non_empty_test) {
    printf("\nGOAL 2c: PASS\n");
    printf("Ignore the (intentional) error.\nIt is because we tested deleting a non-empty directory.\n\n");
    return 1;
  } else {
    printf("\nGOAL 2c: FAIL\n");
    if (file1_before_deletion == NOT_FOUND) printf("File 1 was not created properly.\n");
    if (file1_after_deletion != NOT_FOUND) printf("File 1 was not deleted properly.\n");
    if (file2_before_deletion == NOT_FOUND) printf("File 2 was not created properly.\n");
    if (file2_after_deletion == NOT_FOUND) printf("File 2 was incorrectly deleted.\n");
    if (file3_before_deletion == NOT_FOUND) printf("File 3 was not created properly.\n");
    if (file3_after_deletion == NOT_FOUND) printf("File 3 was incorrectly deleted.\n");
    if (file4_before_deletion == NOT_FOUND) printf("File 4 was not created properly.\n");
    if (file4_after_deletion != NOT_FOUND) printf("File 4 was not deleted properly.\n");
    if (file5_before_deletion == NOT_FOUND) printf("File 5 was not created properly.\n");
    if (file5_after_deletion != NOT_FOUND) printf("File 5 was not deleted properly.\n");
    if (file6_before_deletion == NOT_FOUND) printf("File 6 was not created properly.\n");
    if (file6_after_deletion != NOT_FOUND) printf("File 6 was not deleted properly.\n");
    if (file7_before_deletion == NOT_FOUND) printf("File 7 was not created properly.\n");
    if (file7_after_deletion != NOT_FOUND) printf("File 7 was not deleted properly.\n");
    if (dir1_before_deletion == NOT_FOUND) printf("Directory %s was not created properly.\n", dir1_name);
    if (dir1_after_deletion == NOT_FOUND) printf("Directory %s was incorrectly deleted.\n", dir1_name);
    if (dir2_before_deletion == NOT_FOUND) printf("Directory %s was not created properly.\n", dir2_name);
    if (dir2_after_deletion == NOT_FOUND) printf("Directory %s was incorrectly deleted.\n", dir2_name);
    if (dir3_before_deletion == NOT_FOUND) printf("Directory %s was not created properly.\n", dir3_name);
    if (dir3_after_deletion != NOT_FOUND) printf("Directory %s was not deleted properly.\n", dir3_name);
    if (!non_empty_test) printf("Non-empty directory was deleted incorrectly.\n");
    printf("\n");
    return 0;
  }
}

int simulate_crash() {
  InitLLFS();

  // The following will be written to disk
  execute_touch("/", "test");
  write_str_to_file("/", "test", "this is a test");
  execute_mkdir("/", "foo");
  execute_touch("/foo", "bar");
  write_str_to_file("/foo", "bar", "foobar");

  // The following will be in the buffer when we return, so we will lose it. It will not corrupt any of the files created before.
  execute_touch("/foo", "baz");

  printf("Simulating crash...\n");
  return EXIT_FAILURE; // return while there is still data in the buffer
}

int test_goal3() {
  printf("TESTING GOAL 3: Robustness\n"
         "==========================\n");

  char *block;
  char *test_string = "this is a test";
  int root_inode_index, foo_inode_index, foobar_inode_index;
  DirectoryEntry root[DIRECTORY_CAPACITY];
  DirectoryEntry foo[DIRECTORY_CAPACITY];
  DirectoryEntry foobar[DIRECTORY_CAPACITY];

  int file_text_present, file_present, foo_present, foobar_present; // flags

  simulate_crash();
  AttachLLFS();

  block = malloc(BLOCK_SIZE);
  read_file("/", "test", block);
  file_text_present = strcmp(test_string, block);
  free(block);

  root_inode_index = get_leaf_dir_inode_index("/");
  foo_inode_index = get_leaf_dir_inode_index("/foo");
  foobar_inode_index = get_leaf_dir_inode_index("/foo/bar");
  get_directory_entries_from_inode_index(root_inode_index, root);
  get_directory_entries_from_inode_index(foo_inode_index, foo);
  get_directory_entries_from_inode_index(foobar_inode_index, foobar);

  file_present = strcmp("test", root[0].file_name);
  foo_present = strcmp("foo", root[1].file_name);
  foobar_present = strcmp("bar", foo[0].file_name);

  if (!file_text_present && !file_present && !foo_present && !foobar_present) {
    printf("\nGOAL 3: PASS\n\n");
    return 1;
  } else {
    printf("\nGOAL 3: FAIL\n");
    if (file_text_present != 0 || file_present != 0)
      printf("Test file 'test' was corrupted.\n");
    if (foo_present != 0)
      printf("Test folder '/foo' was corrupted.\n");
    if (foobar_present != 0)
      printf("Test folder '/foo/bar' was corrupted.\n");
    printf("\n");
    return 0;
  }
}

int main() {
  int goal2a = test_goal2a();
  int goal2b = test_goal2b();
  int goal2c = test_goal2c();
  int goal2d = test_goal2d();
  int goal3  = test_goal3();

  int total_tests   = 5;
  int test_passes   = goal2a + goal2b + goal2c + goal2d + goal3;
  int test_failures = total_tests - test_passes;

  printf("\nTEST01 - REPORT\n"
         "==========================================\n");
  printf("Total number of tests: %d\n", total_tests);
  printf("Tests passed: %d\n", test_passes);
  printf("Tests failed: %d\n", test_failures);
  printf("==========================================\n\n");

  return EXIT_SUCCESS;
}