#include "utils.h"

struct data files_left[MAX_FILES], files_right[MAX_FILES];
int size_left = 0, size_right = 0; 
int last_sort_order = 0;   
char last_sort_option[20] = "Nume"; 
char left_history[MAX_HISTORY][PATH_MAX_LEN], right_history[MAX_HISTORY][PATH_MAX_LEN];
int left_top = 0, right_top = 0;
data search_result[MAX_FILES];
int size_search;

void construct_full_path(char *dest, const char *path, const char *filename) {
    strcpy(dest, path);
    dest[PATH_MAX_LEN - 1] = '\0';
    // Verificam si adaugam separatorul de cale (daca lipseste)
    size_t len = strlen(dest);
    if (len > 0 && dest[len - 1] != '/' && dest[len - 1] != '\\') {
        strcat(dest, "\\");
    }
    // Adaugam numele fisierului
    strcat(dest, filename);
}

char lower(char c) {
    if(c >= 'A' && c <= 'Z') return c + 32;
    return c;
}

int compare_strings(char a[], char b[]) {
    if(!a) return -1;
    if(!b) return 1;
    int n = strlen(a);
    if(strlen(b) < n)
        n = strlen(b);
    for(int i = 0; i < n; i++) {
        // ordine: punct/semne, cifre, litere
        if(isalpha(a[i]) && !isalpha(b[i])) 
            return 1;
        else if(!isalpha(a[i]) && isalpha(b[i]))
            return -1;
        else if(!isalpha(a[i]) && !isalpha(b[i])) {
            if(isdigit(a[i]) && !isdigit(b[i]))
                return 1;
            else if(!isdigit(a[i]) && isdigit(b[i]))
                return -1;
            else if(a[i] < b[i]) return -1;
            else if(a[i] > b[i]) return 1;
        }
        else if(lower(a[i]) > lower(b[i])) return 1;
        else if(lower(a[i]) < lower(b[i])) return -1;
    }
    if(strlen(a) == strlen(b)) return 0;
    else if(strlen(a) < strlen(b)) return -1;
    else return 1;
}

char* get_extension(data file) {
    int poz = -1;
    for(int i = strlen(file.name) - 1; i >= 0; i--)
        if(file.name[i] == '.') {
            poz = i;
            break;
        }
    if(poz == -1) return nullptr;
    char *ext = file.name + poz + 1;
    return ext;
}

bool sort_compare(data file1, data file2, char option[]) {
    if(!strcmp(option, "Nume"))
    {
        // nu pot folosi strcmp deoarece 'Z' < 'a'
        int rez = compare_strings(file1.name, file2.name);
        if(rez <= 0) return 0;
        return 1;
    }
    if(!strcmp(option, "Data"))
        return file1.date < file2.date;
    else if(!strcmp(option, "Tip"))
    {
        char *ext1 = get_extension(file1), *ext2 = get_extension(file2);
        int rez = compare_strings(ext1, ext2);
        if(rez <= 0) return 0;
        return 1;
    }
    return file1.size < file2.size;
}

void sort_files(char option[], bool order, data files[], int n) {
    // order - 1 -> descrescator / 0 -> crescator
    strcpy(last_sort_option, option);
    last_sort_order = order;
    for(int i = 0; i < n - 1; i++)
        for(int j = i + 1; j < n; j++)
        // sort_compare returneaza 0 daca files[i] < files[j]
        // una e 1 si una e 0 (crescator si mai mic/invers)
            if(order ^ sort_compare(files[i], files[j], option)) {
                data aux = files[i];
                files[i] = files[j];
                files[j] = aux;
            } 
}

bool compare_strings_search(char a[], char b[]) {
    // verifica daca inceputul a doua siruri (numele fisierelor) sunt aceleasi
    int n = strlen(a);
    if(strlen(b) < n)
        n = strlen(b);
    for(int i = 0; i < n; i++) {
        if (lower(a[i]) != lower(b[i])) return 0;
    }
    return 1;
}

bool strings_search(char a[], char b[]) {
    // cauta sirul b in sirul a
    if(!a) return 0;
    if(!b) return 0;
    char c[PATH_MAX_LEN], d[PATH_MAX_LEN];
    char sep[] = "~!@#$%^&()-_=+[]{};',. ";
    strcpy(c,a);
    strcpy(d,b);
    for (int i = 0; c[i]; ++i) c[i] = lower(c[i]);
    for (int i = 0; d[i]; ++i) d[i] = lower(d[i]);
    // in cazul in care sirul cautat incepe cu separatori, acestia trebuie ignorati
    int i = 0;
    while (strchr(sep,d[i])) i++;
    // in cazul in care sirul cautat se termina cu separatori, acestia trebuie ignorati
    int j = strlen(d)-1;
    while (strchr(sep,d[j])) d[j--] = 0;

    if (strstr(c,d+i) == 0) return 0;
    char *q = strstr(c,d+i);
    char *p = strtok(c,sep);
    while (p != 0) {
        if (compare_strings_search(p,q)) return 1;
        p = strtok(NULL,sep);
    }
    return 0;
}

void search(char a[], const char *path, bool &found) {
    // cauta un fisier intr-un folder dat prin path
    DIR *dp;
    struct dirent *entry;
    dp = opendir(path);
    while ((entry = readdir(dp))) {
        struct stat file_info;
        char full_path[PATH_MAX_LEN];
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        construct_full_path(full_path, path, entry->d_name);
        if (stat(full_path, &file_info) == -1) {
            printf("%s EROARE \n", entry->d_name);
            continue;
        }
        // daca in acel folder se afla un alt folder, verificam daca fisierul cautat se afla in acesta
        if (S_ISDIR(file_info.st_mode)) {
            if (strings_search(entry->d_name, a)) {
                strcpy(search_result[size_search].name, full_path);
                search_result[size_search].isDir = true;
                search_result[size_search].date = file_info.st_mtime;
                search_result[size_search].size = directory_size(full_path);
                size_search++;
                found = 1;
            }
            search(a, full_path, found);
        } 
        else if (strings_search(entry->d_name, a)) {
            strcpy(search_result[size_search].name, full_path);
            search_result[size_search].date = file_info.st_mtime;
            search_result[size_search].isDir = false;
            search_result[size_search].size = (long)file_info.st_size;
            size_search++;
            found = 1;
        }
    }
    closedir(dp);
}


void display_files(data files[], int n) {
    if (n <= 0) {
        printf("Nu exista inregistrari de afisat.\n");
        return;
    }

    printf("\n");
    printf("========================================================================================\n");
    // Antetul tabelului
    // %-xs -> Aliniat la stanga, x caractere
    printf("%-40s | %-15s | %-25s\n", "Nume Fisier", "Dimensiune (Bytes)", "Data/Ora");
    printf("========================================================================================\n");

    // Parcurgerea si afisarea datelor
    for (int i = 0; i < n; i++) {
        // Asiguram ca 'date' este curatat de newline-ul adaugat de ctime (daca e cazul)
        char formatted_time[25];
        strcpy(formatted_time, ctime(&files[i].date));
        formatted_time[24] = '\0';
        size_t len = strlen(formatted_time);
        if (len > 0 && formatted_time[0] == '\n') {
            formatted_time[len - 1] = '\0';
        }
        
        printf("%-40s | %-15ld | %-25s\n", 
               files[i].name, 
               files[i].size, 
               get_extension(files[i]));
    }
    printf("========================================================================================\n");
}


void create_file(const char *path, const char *filename, data files[], int &n) {
    char full_path[PATH_MAX_LEN];
    struct stat file_info;
    construct_full_path(full_path, path, filename);
    if (stat(full_path, &file_info) == 0) {
        fprintf(stdout, "File %s already exists. Skipping creation to prevent overwrite.\n", filename);
        return;
    }
    FILE* file = fopen(full_path, "w");
    if(!file)
        printf("ERROR occured while creating %s", filename);
    else {
        fclose(file);
        if(stat(full_path, &file_info)) {
            printf("ERROR occured while creating file %s", filename);
            return ;
        }
        strcpy(files[n].name, filename);
        files[n].date = time(NULL);
        files[n].size = 0;
        n++;
        sort_files(last_sort_option, last_sort_order, files, n);
    }
}

void create_folder(const char *path, const char *foldername, data files[], int &n) {
    char full_path[PATH_MAX_LEN];
    struct stat dir_info;
    construct_full_path(full_path, path, foldername);
    if(!stat(full_path, &dir_info)) {
        // Exista folder/fisier cu acest nume
        printf("ERROR: %s already exists.", full_path);
    }
    if(_mkdir(full_path) == -1) {
        printf("ERROR: %s creation failed.", full_path);
    }
    else {
        strcpy(files[n].name, foldername);
        files[n].isDir = true;
        files[n].date = time(NULL);
        files[n].size = 0;
        n++;
        sort_files(last_sort_option, last_sort_order, files, n);

    }
}

void file_delete(char *path, char *filename, data files[], int &n) {
    char full_path[PATH_MAX_LEN];
    struct stat file_info;
    int m;
    construct_full_path(full_path, path, filename);
    if(stat(full_path, &file_info) == -1) {
        printf("ERROR: File %s does not exist", filename);
        return;
    }
    if(S_ISDIR(file_info.st_mode)) {
        // trebuie facuta o recursie deoarece rmdir poate sterge doar foldere goale
        data* sub_files = new data[1000];
        char name[PATH_MAX_LEN];
        save_with_metadata(full_path, sub_files, m);
        for(int i = 0; i < m; i++) {
            file_delete(full_path, sub_files[i].name, files, n);
        }
        delete[] sub_files;
        if(_rmdir(full_path) == -1) {
            printf("ERROR: %s creation failed.", full_path);
        }
    }
    else {
        if(remove(full_path)) {
            printf("ERROR: Failed to delete %s", full_path);
            return ;
        }
    }
    for(int i = 0; i < n; i++) {
        if(!strcmp(files[i].name, filename))
        {
            for(int j = i; j < n - 1; j++)
                files[j] = files[j + 1];
            n--;
        }
    }
}



void file_rename(const char *path, const char *old_filename, const char *new_filename, data files[], int &n) {
    char old_full_path[PATH_MAX_LEN];
    char new_full_path[PATH_MAX_LEN];
    construct_full_path(old_full_path, path, old_filename);
    construct_full_path(new_full_path, path, new_filename);
    if(rename(old_full_path, new_full_path)) {
        printf("ERROR: Failed to rename %s to %s in %s", old_filename, new_filename, path);
    }
    else {
        for(int i = 0; i < n; i++)
            if(!strcmp(files[i].name, old_filename))
            {
                strcpy(files[i].name, new_filename);
                sort_files(last_sort_option, last_sort_order, files, n);
                break;
            }
    }
}

void copy(char *path, char *filename, char *dest_path, data dest_files[], int &n, bool &created) {
    char file_path[PATH_MAX_LEN];
    char used_filename[PATH_MAX_LEN];
    char new_file_path[PATH_MAX_LEN];
    struct stat file_info, check;
    int m;
    strcpy(used_filename, filename);
    if (strstr(used_filename, ":\\") != 0) {
        strcpy(file_path, used_filename);
        char *last_slash = strrchr(filename, '\\');
        strcpy(used_filename, last_slash+1);
    }
    else {
        construct_full_path(file_path, path, used_filename);
    }
    construct_full_path(new_file_path, dest_path, used_filename);
    if (stat(new_file_path, &check) == 0) {
        fprintf(stdout, "File %s already exists. \n", used_filename);
        created = 0;
        return;
    }
    if (stat(file_path, &file_info) != 0) {
        printf("ERROR: Could not get status for %s\n", file_path);
        return; 
    }
    if(S_ISDIR(file_info.st_mode)) {
        create_folder(dest_path, used_filename, dest_files, n);
        data* sub_files = new data[1000];
        save_with_metadata(file_path, sub_files, m);
        for(int i = 0; i < m; i++) {
            copy(file_path, sub_files[i].name, new_file_path, dest_files, n, created);
        }
        delete[] sub_files;
    }
    else {
        FILE *file, *dest_file;
        char buffer[4096];
        size_t bytes_read;
        file = fopen(file_path, "rb");
        if (file == NULL) {
            printf("Error opening source file");
            exit(1);
        }
        dest_file = fopen(new_file_path, "wb");
        if (dest_file == NULL) {
            printf("Error opening destination file");
            exit(1);
        }
        while ((bytes_read = fread(buffer, 1, 4096, file)) > 0) {
            if (fwrite(buffer, 1, bytes_read, dest_file) != bytes_read) {
                printf("Error writing to destination file");
                exit(1);
            }
        }
        
        fclose(file);
        fclose(dest_file);
    }
    save_with_metadata(dest_path, dest_files, n);
    sort_files(last_sort_option, last_sort_order, dest_files, n);
}

void move(char *path, char *filename, char *dest_path, data files1[], int &n1, data files2[], int &n2, bool &created) {
    copy(path, filename, dest_path, files2, n2, created);
    if (created) file_delete(path, filename, files1, n1);
    sort_files(last_sort_option, last_sort_order, files1, n1);
    sort_files(last_sort_option, last_sort_order, files2, n2);
}

long directory_size(char *path) {
    DIR *dir;
    struct dirent *entry;
    struct stat file_info;
    char full_path[PATH_MAX_LEN];
    long size = 0;
    if ((dir = opendir (path)) != NULL) {
        while ((entry = readdir (dir)) != NULL) {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
            construct_full_path(full_path, path, entry->d_name);
            if(stat(full_path, &file_info) == -1) {
                continue;
            }
            if (S_ISDIR(file_info.st_mode)) {
                size += directory_size(full_path);
            }
            else {
                size += (long)file_info.st_size;
            }
        
        } 
    }
    else {
        closedir (dir);
        printf("Eroare la deschiderea directorului");
        return 0;
    }
    closedir (dir);
    return size;
}


void save_with_metadata(const char *path, data files[], int &n) {
    DIR *dp;
    struct dirent *entry;

    // 1. Deschidem directorul
    dp = opendir(path);
    if (dp == NULL) {
        printf("Eroare la deschiderea directorului %s", path);
        return;
    }

    // printf("\nScanez: %s\n", path);
    // printf("----------------------------------------------------------------------------------------------------\n");
    // printf("%-30s | %-10s | %-15s | %-24s\n", "Nume", "Tip", "Dimensiune (Bytes)", "Data Ultima Modificare");
    // printf("----------------------------------------------------------------------------------------------------\n");

    // 2. Parcurgem intrarile directorului cu readdir()
    n = 0;
    while ((entry = readdir(dp))) {
        struct stat file_info;
        char full_path[PATH_MAX_LEN];
        char formatted_time[25];
        
        // Ignoram intrarile speciale "." si ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        construct_full_path(full_path, path, entry->d_name);
        
        // B. APELAREA stat() PENTRU METADATE
        if (stat(full_path, &file_info) == -1) {
            // Daca nu putem citi detaliile (ex: permisiuni insuficiente)
            printf("%-30s | EROARE    | N/A             | N/A\n", entry->d_name);
            continue;
        }

        // Determinarea tipului (Folder sau Fisier)
        char type_str[10];
        long size;
        bool isDir = false;
        if (S_ISDIR(file_info.st_mode)) {
            size = directory_size(full_path);
            isDir = true;
            strcpy(type_str, "[DIR]");
        } else {
            size = (long)file_info.st_size;
            strcpy(type_str, "Fisier");
        }
        strcpy(files[n].name, entry->d_name);
        files[n].date = file_info.st_mtime;
        files[n].size = size;
        files[n].isDir = isDir;

        n++;
    } 
    sort_files(last_sort_option, last_sort_order, files, n);
    closedir(dp);
}

char* get_executable_directory() {
    char exe_path[PATH_MAX_LEN];
    DWORD path_len_dw;
    
    // Obtinem calea absoluta a intregului executabil
    path_len_dw = GetModuleFileNameA(NULL, exe_path, PATH_MAX_LEN);

    if (path_len_dw == 0 || path_len_dw >= PATH_MAX_LEN) {
        fprintf(stderr, "ERROR: Failed to get executable path. Length: %lu\n", (unsigned long)path_len_dw);
        return NULL;
    }
    // Gasim ultimul separator (\)
    char* last_slash = strrchr(exe_path, '\\');
    if (last_slash != NULL) {
        *last_slash = '\0';
    } else {
        // Daca nu gasim separator, executabilul este in root (ex: in "C:\")
        exe_path[0] = '\0';
    }
    size_t dir_len = strlen(exe_path);
    char* dir_path = (char*)malloc(dir_len + 1);
    if (dir_path == NULL) {
        fprintf(stderr, "ERROR: Memory allocation failed for executable directory path.\n");
        return NULL;
    }
    strcpy(dir_path, exe_path);
    return dir_path;
}

void navigate(char *current_path, const char *target_name, data files[], int &size) {
    if (strcmp(target_name, "..") == 0) {
        char *last_slash = strrchr(current_path, '\\');
        if (last_slash) {
            *last_slash = '\0';
        }
        if (strlen(current_path) < 3) {
            strcat(current_path, "\\");
        }
    } 
    else if (strcmp(current_path, target_name) != 0){
        if (strlen(current_path) == 3) {
            strcat(current_path, target_name);
        }
        else {
            strcat(current_path, "\\");
            strcat(current_path, target_name);
        }
    }
    save_with_metadata(current_path, files, size);
}

void open_file(const char *folder_path, const char *file_name) {
    char full_path[PATH_MAX_LEN];
    if(!strstr(file_name, folder_path)) {
        // daca in full_path nu este deja file_name
        strcpy(full_path, folder_path);
        strcat(full_path, "\\");
        strcat(full_path, file_name);
    }
    else strcpy(full_path, file_name);
    ShellExecuteA(NULL, "open", full_path, NULL, NULL, SW_SHOWNORMAL);
}

char* convert_size(long bytes) {
    char *buffer;
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int index = 0;
    double size = bytes;
    while(size >= 1024 && index < 4) {
        size /= 1024;
        index++;
    }  
    snprintf(buffer, 32*sizeof(char), "%.2f %s", size, units[index]);
    return buffer;
}