#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

#include "util.h"
#include "files.h"

LS_RESULT getFiles(char* dirname, bool show_hidden) {
    LS_RESULT result;
    result.num = -1;

    // Fill directory array with entries sorted alphabetically, as well as number of entries
    if ((result.num = scandir(dirname, &result.dirp, NULL, alphasort)) < 0) {
        fprintf(stderr, "brils: cannot open %s\n", dirname);
        return result;
    } else {
        // Change directory to ls directory
        chdir(dirname);

        if (!show_hidden) {
            // Get number of non-hidden files
            int shown_num = 0;
            for (int i = 0; i < result.num; i++) {
                if (result.dirp[i]->d_name[0] != '.') {
                    shown_num++;
                }
            }

            // Fill files array with only shown files
            result.files = (struct stat *) malloc(shown_num * sizeof(struct stat));
            int shown_index = 0;
            for (int i = 0; i < result.num; i++) {
                if (result.dirp[i]->d_name[0] != '.') {
                    struct stat st;
                    lstat(result.dirp[i]->d_name, &st);
                    result.files[shown_index++] = st;
                }
            }
            result.num = shown_num;
        } else {
            // Fill array with all files
            result.files = (struct stat *) malloc(result.num * sizeof(struct stat));
            for (int i = 0; i < result.num; i++) {
                struct stat st;
                lstat(result.dirp[i]->d_name, &st);
                result.files[i] = st;
            }
        }

    }

    return result;
}