#ifndef ZOS_MAIN_H
#define ZOS_MAIN_H

#include <stdint.h>
#include <stdio.h>

#define FS_TEMP_FILE "inode.dat"
#define SPLITTER " "
#define COMMAND_LEN 100
#define OK       0
#define NO_INPUT 1
#define TOO_LONG 2


#define SIGNATURE "JANELLE"
#define DESCRIPTOR "ZOS-2019"

// ================================== [COMMANDS] ==================================
#define COPY_FILE "cp"
#define MOVE_FILE "mv"
#define REMOVE_FILE "rm"
#define MAKE_DIRECTORY "mkdir"
#define REMOVE_EMPTY_DIRECTORY "rmdir"
#define PRINT_DIRECTORY "ls"
#define PRINT_FILE "cat"
#define MOVE_TO_DIRECTORY "cd"
#define ACTUAL_DIRECTORY "pwd"
#define MFT_ITEM_INFO "info"
#define HD_TO_PSEUDO "incp"
#define PSEUDO_TO_HD "outcp"
#define LOAD_COMMANDS "load"
#define FILE_FORMATTING "format"
#define CONTROL_KONZ "check"
#define EXIT "exit"
#define HELP "help"

#define ROOT_NAME "root"

// ================================== [CONSTANT] ==================================
#define MAX_INODE_COUNT = 100
#define INODES_BLOCK_SIZE_RATIO 0.1
#define DISK_SIZE 102400
#define CLUSTER_SIZE 4096
#define MAX_DIRECT_LINKS 5
#define MAX_NAME_LEN 12
#define DIRECTORY 1



// ================================== [FILESYSTEM] ==================================

#define FILE_NF "FILE NOT FOUND"
#define PATH_NF "PATH NOT FOUND"
#define EXIST "EXIST"
#define NE "NOT EMPTY"
#define CCFILE "CANNOT CREATE FILE"

//int MAX_FILES_IN_DIR = CLUSTER_SIZE / sizeof(int32_t);

typedef struct superblock SUPERBLOCK;
typedef struct pseudo_inode INODE;
typedef struct my_inode_blocks INODE_BLOCK;
typedef struct my_bitmap BITMAP;
typedef struct my_vfs VFS;
typedef struct directory_item DIR_ITEM;
typedef struct directory DIR;

struct superblock {
    char signature[9];              //login autora FS
    char volume_descriptor[251];    //popis vygenerovaného FS
    int32_t disk_size;              //celkova velikost VFS
    int32_t cluster_size;           //velikost clusteru
    int32_t cluster_count;          //pocet clusteru
    int32_t inode_start_address;    //adresa pocatku  i-uzlů
    int32_t bitmap_start_address;   //adresa pocatku bitmapy datových bloků
    int32_t data_start_address;     //adresa pocatku datovych bloku
};

struct my_inode_blocks {
    int32_t size;
    INODE **items;
};

struct pseudo_inode {
    int32_t nodeid;                 //ID i-uzlu, pokud ID = ID_ITEM_FREE, je polozka volna
    int isDirectory;                //soubor, nebo adresar
    int8_t references;              //počet odkazů na i-uzel, používá se pro hardlinky
    int32_t file_size;              //velikost souboru v bytech
    int32_t direct[MAX_DIRECT_LINKS]; // příme odkazy na datové bloky
    int32_t indirect1;              // 1. nepřímý odkaz (odkaz - datové bloky)
    int32_t indirect2;              // 2. nepřímý odkaz (odkaz - odkaz - datové bloky)
    int cluster_count;
};

struct directory {
    DIR_ITEM **files;
};

struct directory_item {
    int32_t inode;                   // inode odpovídající souboru
    char item_name[12];              //8+3 + /0 C/C++ ukoncovaci string znak
};

struct my_bitmap {
    int32_t length;
    unsigned char *data;
};

struct my_vfs {
    SUPERBLOCK *superblock;
    INODE_BLOCK *inode_blocks;
    BITMAP *bitmap;

    char *actual_path;
    char *filename;
    FILE *FILE;
};

struct the_fragment_temp {
    int successful;
    int32_t start_cluster_ID;
    int count;
};

// MAIN
void hello();
int compare_two_string(char *string1, char *string2);
int getLine (char *prmpt, char *buff, size_t sz);

// BITMAP
int32_t get_one_free_cluster(BITMAP **bitmap);
int check_free_clusters (BITMAP **bitmap, int count);
void bitmap_init(BITMAP **bitmap, int32_t cluster_count);
int bitmap_contains_free_cluster(BITMAP *bitmap);
int used_clusters(BITMAP *bitmap);
void bitmap_info(BITMAP *bitmap);
struct the_fragment_temp *find_free_cluster(BITMAP **bitmap, int needed_count);


// VIRTUAL FILESYSTEM
void vfs_init(VFS **vfs, char *filename, size_t disk_size);
void create_vfs_file(VFS **vfs);

// SUPERBLOCK
void superblock_init(SUPERBLOCK **superblock, int32_t disk_size, int32_t cluster_size);
void superblock_info(SUPERBLOCK *superblock);

// INODE BLOCKS
void inode_blocks_init(VFS **vfs);
void inode_init(VFS **vfs, int node_id, char *name, int isDirectory, int item_size);
INODE *get_inode_from_path(VFS *vfs, char *tok);
void fill_data_block_directory(VFS **vfs, int cluster_count);
void fill_data_block_file(VFS **vfs, int cluster_count);

#endif //ZOS_MAIN_H
