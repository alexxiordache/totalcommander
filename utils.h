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
#include <SFML/Graphics.hpp>
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PATH_MAX_LEN 1024
#define MAX_FILES 1000 

struct data{
    char name[PATH_MAX_LEN];
    time_t date;
    long size;
    bool isDir;
} ;

extern struct data files_left[MAX_FILES], files_right[MAX_FILES];
extern int last_sort_order;
extern int size_left, size_right;
extern char last_sort_option[30];
// time_t now(); 
void construct_full_path(char *dest, const char *path, const char *filename);
char lower(char c);

// Array Management & Sorting
void sort_files(char option[], bool order, struct data files[], int n);
bool sort_compare(struct data file1, struct data file2, char option[]); // Use struct data

// File/Path Utilities
char* get_extension(struct data file); // Use struct data
long directory_size(char *path); 

// File/Folder I/O
void save_with_metadata(const char *path, struct data files[], int &n); 
void create_file(const char *path, const char *filename, struct data files[], int &n);
void create_folder(const char*path, const char *foldername, struct data files[], int &n);
void file_delete(char* path, char* filename, struct data files[], int &n);
void file_rename(const char* path, const char* old_filename, const char* new_filename, struct data files[], int &n);
void copy(char* path, char* filename, char* dest_path, struct data files[], int &n);
void move(char* path, char* filename, char* dest_path, struct data files1[], int &n1, struct data files2[], int &n2);

// Searching
bool compare_strings(const char a[], const char b[]);
bool compare_strings_search(const char a[], const char b[]);
bool strings_search(const char a[], const char b[]);
void search(char a[], const char *path, bool &found); 
void display_files(struct data files[], int n);
char* get_executable_directory();

#ifdef __cplusplus
}
#endif

#endif // UTILS_H