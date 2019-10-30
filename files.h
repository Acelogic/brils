#ifndef BRILS_FILES_H
#define BRILS_FILES_H

typedef struct ls_result {
    // Array of directory entries
    struct dirent **dirp;

    // Number of files in the directory
    int num;

    // Array of files
    struct stat* files;
} LS_RESULT;

LS_RESULT getFiles(char* dirname, bool show_hidden);

#endif //BRILS_FILES_H
