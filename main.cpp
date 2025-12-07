#include <dirent.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <sys/stat.h>

const int PATH_MAX_LEN = 1024;


struct data{
    char name[PATH_MAX_LEN];
    time_t date;
    long size;
} files[1000];

int last_sort_order = 0, n;
char last_sort_option[30]="Nume";

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
        if(lower(a[i]) > lower(b[i])) return 1;
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
    if(!strcmp(option, "Data Modificarii"))
        return file1.date < file2.date;
    else if(!strcmp(option, "Tipul Fisierului"))
    {
        char *ext1 = get_extension(file1), *ext2 = get_extension(file2);
        printf("%s, %s\n", ext1, ext2); 
        int rez = compare_strings(ext1, ext2);
        if(rez <= 0) return 0;
        return 1;
    }
    else if(!strcmp(option, "Dimensiune"))
        return file1.size < file2.size;
}

void sort_files(char option[], bool order) {
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


void display_files() {
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
        strncpy(formatted_time, ctime(&files[i].date), sizeof(formatted_time) - 1);
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


void construct_full_path(char *dest, const char *path, const char *filename) {
    // Copiem calea de baza (ex: /home/user)
    strcpy(dest, path);
    dest[PATH_MAX_LEN - 1] = '\0';
    
    // Verificam si adaugam separatorul de cale (daca lipseste)
    size_t len = strlen(dest);
    if (len > 0 && dest[len - 1] != '/' && dest[len - 1] != '\\') {
        strncat(dest, "/", PATH_MAX_LEN - len);
    }
    
    // Adaugam numele fisierului
    // strncat(dest, filename, PATH_MAX_LEN - strlen(dest));
    strcat(dest, filename);
}

void create_file(const char *path, const char *filename) {
    char full_path[PATH_MAX_LEN];
    construct_full_path(full_path, path, filename);
    FILE* file = fopen(full_path, "w");
    if(!file)
        printf("ERROR occured while creating %s", filename);
    else {
        struct stat file_info;
        fclose(file);
        stat(full_path, &file_info);
        strcpy(files[++n].name, filename);
        files[n].date = file_info.st_mtime;
        files[n].size = (long) file_info.st_size;
        sort_files(last_sort_option, last_sort_order);
    }
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
        perror ("");
        return EXIT_FAILURE;
    }
    closedir (dir);
    return size;
}


void save_with_metadata(const char *path) {
    DIR *dp;
    struct dirent *entry;

    // 1. Deschidem directorul
    dp = opendir(path);
    if (dp == NULL) {
        perror("Eroare la deschiderea directorului");
        return;
    }

    // printf("\nScanez: %s\n", path);
    // printf("----------------------------------------------------------------------------------------------------\n");
    // printf("%-30s | %-10s | %-15s | %-24s\n", "Nume", "Tip", "Dimensiune (Bytes)", "Data Ultima Modificare");
    // printf("----------------------------------------------------------------------------------------------------\n");

    // 2. Parcurgem intrarile directorului cu readdir()
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
        if (S_ISDIR(file_info.st_mode)) {
            size = directory_size(full_path);
            strcpy(type_str, "[DIR]");
        } else {
            size = (long)file_info.st_size;
            strcpy(type_str, "Fisier");
        } 
        // Formatarea timpului (Data Modificarii)
        // ctime returneaza un string cu newline la final, asa ca il indepartam


        // POSIBILA REFOLOSIRE
        // strncpy(formatted_time, ctime(&file_info.st_mtime), sizeof(formatted_time) - 1);
        // formatted_time[24] = '\0'; // Asiguram terminarea sirului
        
        strcpy(files[n].name, entry->d_name);
        files[n].date = file_info.st_mtime;
        files[n].size = size;
        n++;
    }
    sort_files(last_sort_option, last_sort_order);
    printf("----------------------------------------------------------------------------------------------------\n");
    closedir(dp);
}

int main() {

    DIR *dir;
    struct dirent *ent;
    int n = 0;
    char path[PATH_MAX_LEN]="C:\\Users\\alex\\Documents\\c++\\Total Commander\\test";
    save_with_metadata(path);
    // sort_files("Nume", 0);
    create_file(path, "ff.txt");
    display_files();
//     if ((dir = opendir (path)) != NULL) {
//     while ((ent = readdir (dir)) != NULL) {
//         strcpy(fisiere[n++], ent->d_name);
//         printf ("%s, %s\n", ent->d_name, ent->d_reclen);
//     }
//     closedir (dir);
//     } else {
//     /* could not open directory */
//     perror ("");
//     return EXIT_FAILURE;
// }
}

// la sortare vezi ce faci cu folderele; la type foldere primele [DONE]
// sortare: nume, data, dimensiune, extensie? [DONE]
// filtrare
//cautare
// Dupa ce se face o modificare in folder, trebuiesc rearanjate toate
// TODO: La create file nu se adauga in vector (cred)