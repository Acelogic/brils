#include <stdio.h>
#include <sys/stat.h>

#include "util.h"

void printType(int type) {
    switch(type) {
        case S_IFSOCK:
            printf("s");
            break;
        case S_IFLNK:
            printf("l");
            break;
        case S_IFREG:
            printf("-");
            break;
        case S_IFBLK:
            printf("b");
            break;
        case S_IFDIR:
            printf("d");
            break;
        case S_IFCHR:
            printf("c");
            break;
        case S_IFIFO:
            printf("p");
            break;
        default:
            printf("?");
            break;
    }
}

void printPerms(int perm) {
    if (perm & S_IRUSR) {
        printf("r");
    }
    else {
        printf("-");
    }
    if (perm & S_IWUSR) {
        printf("w");
    }
    else {
        printf("-");
    }
    if (perm & S_IXUSR) {
        if (perm & S_ISUID) {
            printf("s");
        }
        else {
            printf("x");
        }
    }
    else {
        if (perm & S_ISUID) {
            printf("S");
        }
        else {
            printf("-");
        }
    }
    if (perm & S_IRGRP) {
        printf("r");
    }
    else {
        printf("-");
    }
    if (perm & S_IWGRP) {
        printf("w");
    }
    else {
        printf("-");
    }
    if (perm & S_IXGRP) {
        if (perm & S_ISGID) {
            printf("s");
        }
        else {
            printf("x");
        }
    }
    else {
        if (perm & S_ISGID) {
            printf("S");
        }
        else {
            printf("-");
        }
    }
    if (perm & S_IROTH) {
        printf("r");
    }
    else {
        printf("-");
    }
    if (perm & S_IWOTH) {
        printf("w");
    }
    else {
        printf("-");
    }
    if (perm & S_IXOTH) {
        if (perm & S_ISVTX) {
            printf("t");
        }
        else {
            printf("x");
        }
    }
    else {
        if (perm & S_ISVTX) {
            printf("T");
        }
        else {
            printf("-");
        }
    }
}

int getDigits(unsigned long long num) {		// Get number of digits in number
    int i = 0;
    unsigned long long int result = num;
    while (result > 0) {
        i++;
        result /= 10;
    }
    return i;
}
