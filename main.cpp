#include "utils.h"

int main() {
    DIR *dir;
    struct dirent *ent;
    bool found = 0;
    char path[PATH_MAX_LEN]="C:\\Users\\alex\\Documents\\c++\\Total Commander\\test";
    char nume[]="dir";
    save_with_metadata(path, files, n);
    // file_delete(path, nume);
    // sort_files("Nume", 0);
    //create_file(path, "ff.txt");
    display_files();
    // search("afa%$&",path, found);
    // if (!found) {
    //     printf("No items match your search.");
    // }
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