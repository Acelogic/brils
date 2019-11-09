#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <sys/stat.h>

#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>
#include <grp.h>
#include <pwd.h>

#include "util.h"
#include "files.h"

// Macros
#define HAS_ARG(x) (args != NULL && strchr(args, x))

// Colors
#define COLOR_RED printf("\033[31m")
#define COLOR_GREEN printf("\033[32m")
#define COLOR_BLUE printf("\033[34m")
#define COLOR_YELLOW printf("\033[33m")
#define COLOR_MAGENTA printf("\033[35m")
#define COLOR_CYAN printf("\033[36m")
#define COLOR_WHITE printf("\033[37m")
#define COLOR_BLACK printf("\033[30m")
#define COLOR_DEFAULT printf("\033[39m")

#define COLOR_BOLD printf("\033[1m")
#define COLOR_ITALIC printf("\033[3m")
#define COLOR_UNDERLINE printf("\033[4m")
#define COLOR_INVERSE printf("\033[7m")
#define COLOR_STRIKE printf("\033[9m")
#define COLOR_OFF printf("\033[0m")

void do_ls_args(char *dirname, char *args, char multi);
void printL(struct stat st, int size_length, int owner_length, int group_length, int link_size, bool mode);

int main(int argc, char **argv) {
    chdir(getcwd(NULL, 0));	// Set current directory

    int opt = 0;
    char *args = (char *) malloc(16 * sizeof(char));	// Allocate args string
    while((opt = getopt(argc, argv, "lafh")) != -1) {	// Get all option arguments
    const char *humanSize(unsigned long long int bytes);
        switch (opt) {
            case 'l':		// List mode
                strcat(args, "l");
                break;
            case 'a':		// Show all files
                strcat(args, "a");
                break;
            case 'f':		// Label directories with a '/'
                strcat(args, "f");
                break;
            case 'h':       // Human Readable File Size
                strcat(args, "h");          
            default:		// Invalid argument
                exit(-1);
        }
    }
    if (optind != argc) {	// If directory(s) specified
        for (; optind < argc; optind++) {
            do_ls_args(argv[optind], args, 1);	// Run ls on each directory
        }
    }
    else {					                    // Otherwise
        do_ls_args(".", args, 0);				// Run ls on current directory
    }

    return(0);
}

// Takes the directory, the arguments, and a boolean to show the directory name
void do_ls_args(char *dirname, char *args, char multi) {
	LS_RESULT result = getFiles(dirname, HAS_ARG('a'));

	if (result.num == -1) {
	    return;
	}

    // Sizes of strings
    int size_length = 0;
    int owner_length = 0;
    int group_length = 0;
    int link_size = 0;

    int term_w;	// Terminal width

    if (HAS_ARG('l')) {		// If list mode, calculate max lengths of strings
        for (int i = 0; i < result.num; i++) {
            // Get max size length
            if (getDigits(result.files[i].st_size) > size_length) {
                size_length = getDigits(result.files[i].st_size);
            }

            // Get max owner length
            char *owner = getpwuid(result.files[i].st_uid) -> pw_name;
            if (strlen(owner) > owner_length) {
                owner_length = strlen(owner);
            }

            // Get max group length
            char *group = getgrgid(result.files[i].st_gid) -> gr_name;
            if (strlen(group) > group_length) {
                group_length = strlen(group);
            }

            // Get max link size
            if (getDigits(result.files[i].st_nlink) > link_size) {
                link_size = getDigits(result.files[i].st_nlink);
            }
        }
    }
    else {	// If non-list mode, get terminal width
        struct winsize t;
        ioctl(0, TIOCGWINSZ, &t);
        term_w = t.ws_col;
    }

    if (multi) {		// If showing directory name(s)
        COLOR_WHITE;
        COLOR_BOLD;
        COLOR_UNDERLINE;
        printf("%s:\n", dirname);
        COLOR_OFF;
    }

    // Loop to print file listing
    int line_length = 0;
    for (int i = 0; i < result.num; i++) {
        COLOR_BOLD;
        
        // If list mode, print file details
        if (HAS_ARG('l')) {
            if(HAS_ARG('h')){ 
                printL(result.files[i], size_length, owner_length, group_length, link_size, 0);
            }
            else
            {
                printL(result.files[i], size_length, owner_length, group_length, link_size, 1);
            }    
        }
      
        // File Name
        COLOR_OFF;

        // If list mode
        if (HAS_ARG('l')) {
            printf("%s", result.dirp[i] -> d_name);
            if (((result.files[i].st_mode >> 12) << 12) == S_IFDIR && HAS_ARG('f')) {	// If labeling directories
                COLOR_BLUE;
                COLOR_BOLD;
                printf("/");
                COLOR_DEFAULT;
            }
            printf("\n");
        }

        // If not list mode
        else {
            if (line_length + strlen(result.dirp[i] -> d_name) + 2 < term_w) {			// If output fits within terminal width
                printf("%s", result.dirp[i] -> d_name);
                if (((result.files[i].st_mode >> 12) << 12) == S_IFDIR && HAS_ARG('f')) {
                    COLOR_BLUE;
                    COLOR_BOLD;
                    printf("/");
                    COLOR_DEFAULT;
                    line_length++;
                }
                line_length += strlen(result.dirp[i] -> d_name);
                printf("  ");
                line_length += 2;
            }
            else if (line_length + strlen(result.dirp[i] -> d_name) < term_w) {		// If output barely fits
                printf("%s", result.dirp[i] -> d_name);
                if (((result.files[i].st_mode >> 12) << 12) == S_IFDIR && HAS_ARG('f')) {
                    COLOR_BLUE;
                    COLOR_BOLD;
                    printf("/");
                    COLOR_DEFAULT;
                    line_length++;
                }
                line_length += strlen(result.dirp[i] -> d_name);

                int c;
                for (c = line_length; c < term_w; c++) {
                    printf(" ");
                }

                line_length = 0;	// Reset
                COLOR_OFF;
                printf("\n");
            }
            else {		// If output does not fit
                line_length = 0;	// Reset
                printf("\n");
            }
        }

        // Cleanup
        //free(result.dirp[i]);
    }

    //free(dirp);
    if (!HAS_ARG('l')) {	// If list mode
        printf("\n");
    }
}

 const char *humanSize(unsigned long long int bytes)
{
	char *suffix[] = {"B", "KB", "MB", "GB", "TB", "PB", "EB"};
	char length = sizeof(suffix) / sizeof(suffix[0]);

	int i = 0;
	double dblBytes = bytes;

	if (bytes > 1024) {
		for (i = 0; (bytes / 1024) > 0 && i<length-1; i++, bytes /= 1024)
			dblBytes = bytes / 1024;
	}

	static char output[200];
	sprintf(output, "%.00lf%s", dblBytes, suffix[i]);
	return output;
}


void printL(struct stat st, int size_length, int owner_length, int group_length, int link_size, bool mode) {
    // Permissions (octal)
    int perms =   (st.st_mode & S_IRUSR)
                | (st.st_mode & S_IWUSR)
                | (st.st_mode & S_IXUSR)
                | (st.st_mode & S_IRGRP)
                | (st.st_mode & S_IWGRP)
                | (st.st_mode & S_IXGRP)
                | (st.st_mode & S_IROTH)
                | (st.st_mode & S_IWOTH)
                | (st.st_mode & S_IXOTH)
                | (st.st_mode & S_ISUID)
                | (st.st_mode & S_ISGID)
                | (st.st_mode & S_ISVTX);
    COLOR_MAGENTA;
    printf("%-.4o ", perms);			// Print octal permissions
    COLOR_DEFAULT;

    // File type
    printType((st.st_mode >> 12) << 12);

    // Permissions (human-readable)
    printPerms(st.st_mode & 0007777);
    printf(" ");
    COLOR_DEFAULT;

    // Number of Links
    COLOR_GREEN;
    printf("%-*d", link_size + 1, (int) st.st_nlink);
    COLOR_DEFAULT;

    // Owner
    COLOR_CYAN;
    printf("%-*s", owner_length + 1, getpwuid(st.st_uid) -> pw_name);
    COLOR_DEFAULT;

    // Group
    COLOR_BLUE;
    printf("%-*s", group_length + 1, getgrgid(st.st_gid) -> gr_name);
    COLOR_DEFAULT;

    
    if(mode == 0){ 
    // File Size
    COLOR_RED;
    printf("%*llu ", size_length, (unsigned long long int) st.st_size);
    COLOR_DEFAULT;
    }
    else { 
     // File Size Human Readable
     COLOR_RED;
     printf("%*s ", size_length, humanSize( (unsigned long long int) st.st_size));
     COLOR_DEFAULT; 
    }

    // Last Modification Date
    char buffer[16];
    strftime(buffer, 16, "%b %e %R", localtime(&st.st_mtime));
    COLOR_YELLOW;
    printf("%s ", buffer);
    COLOR_DEFAULT;
}