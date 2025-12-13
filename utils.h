#ifndef UTILS_H
#define UTILS_H

#include <dirent.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <sys/stat.h>
#include <unistd.h>
#include <direct.h>
#include <stdio.h>


#ifdef __cplusplus
extern "C" {
#endif

#define PATH_MAX_LEN 1024
#define MAX_FILES 1000 

struct data{
    char name[PATH_MAX_LEN];
    time_t date;
    long size;
} ;

// --- EXTERNAL GLOBAL VARIABLES (Declared here, Defined only once in utils.cpp) ---
extern struct data files[MAX_FILES]; // Use struct data
extern int last_sort_order;
extern int n;
extern char last_sort_option[30];

// --- FUNCTION PROTOTYPES (Declarations) ---

// Utility
time_t now(); // <--- ADDED MISSING PROTOTYPE
void construct_full_path(char *dest, const char *path, const char *filename);
char lower(char c);

// Array Management & Sorting
void sort_files(char option[], bool order);
bool sort_compare(struct data file1, struct data file2, char option[]); // Use struct data

// File/Path Utilities
char* get_extension(struct data file); // Use struct data
long directory_size(char *path); 

// File/Folder I/O
void save_with_metadata(const char *path, struct data files[], int &n); 
void create_file(const char *path, const char *filename);
void create_folder(const char*path, const char *foldername);
void file_delete(char* path, char* filename);
void file_rename(const char* path, const char* old_filename, const char* new_filename);
void copy(char* path, char* filename, char* dest_path);
void move(char* path, char* filename, char* dest_path);

// Searching
bool compare_strings(const char a[], const char b[]);
bool compare_strings_search(const char a[], const char b[]);
bool strings_search(const char a[], const char b[]);
void search(char a[], const char *path, bool &found); 
void display_files();

#ifdef __cplusplus
}
#endif

#endif // UTILS_H