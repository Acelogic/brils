#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>

// File extension colors

// Macros
#define HAS_ARG(x) strchr(args, x)

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

// Command
void do_ls_args(char *, char *, char);	// Takes the directory, the arguments, and a boolean to show the directory name

// Utils
int getDigits(unsigned long long int);	// Gets number of digits in number

int main(int argc, char **argv) {
	chdir(getcwd(NULL, 0));	// Set current directory
	
	int opt = 0;
	char *args = (char *) malloc(16 * sizeof(char));	// Allocate args string
	while((opt = getopt(argc, argv, "laf")) != -1) {	// Get all option arguments
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
			default:		// Invalid argument
				exit(-1);
		}
	}
	if (optind != argc) {	// If directory(s) specified
		for (; optind < argc; optind++) {
			do_ls_args(argv[optind], args, 1);	// Run ls on each directory
		}
	}
	else {					// Otherwise
		do_ls_args(".", args, 0);				// Run ls on current directory
	}
	
	return(0);
}

void do_ls_args(char *dirname, char *args, char multi) {
	struct dirent **dirp  = NULL;	// Array of directory entries
	int num;	// Number of files in the directory

	if ((num = scandir(dirname, &dirp, NULL, alphasort)) < 0) {		// Fill array with entries sorted alphabetically, as well as number of entries
		fprintf(stderr, "ls: cannot open %s\n", dirname);	// If error
	}
	else {
		int i;
		chdir(dirname);	// Change directory to ls directory

		// Sizes of strings
		int size_length = 0;
		int owner_length = 0;
		int group_length = 0;
		int link_size = 0;

		int term_w;	// Terminal width

		if (args != NULL && HAS_ARG('l')) {		// If list mode, calculate max lengths of strings
			for (i = 0; i < num; i++) {
				if (!HAS_ARG('a') && dirp[i] -> d_name[0] == '.') {	// If not in show all mode
					continue;
				}	
					
				struct stat st;					// File stat struct
				lstat(dirp[i] -> d_name, &st);	// Get stat of current file

				// Get max size length
				if (getDigits(st.st_size) > size_length) {
					size_length = getDigits(st.st_size);
				}

				// Get max owner length
				char *owner = getpwuid(st.st_uid) -> pw_name;
				if (strlen(owner) > owner_length) {
					owner_length = strlen(owner);
				}

				// Get max group length
				char *group = getgrgid(st.st_gid) -> gr_name;
				if (strlen(group) > group_length) {
					group_length = strlen(group);
				}

				// Get max link size
				if (getDigits(st.st_nlink) > link_size) {
					link_size = getDigits(st.st_nlink);
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
		for (i = 0; i < num; i++) {
			if (((args == NULL || !HAS_ARG('a')) && dirp[i] -> d_name[0] == '.')) {	// If skipping hidden files
				continue;
			}

			// File Stats
			struct stat st;					// File stat struct
			lstat(dirp[i] -> d_name, &st);	// Get file stats
				
			COLOR_BOLD;
			if (args != NULL && HAS_ARG('l')) {		// If list mode, print file details
				
				// Permissions (octal)
				int perms = (st.st_mode & S_IRUSR) | (st.st_mode & S_IWUSR) | (st.st_mode & S_IXUSR) | (st.st_mode & S_IRGRP) | (st.st_mode & S_IWGRP) | (st.st_mode & S_IXGRP) | (st.st_mode & S_IROTH) | (st.st_mode & S_IWOTH) | (st.st_mode & S_IXOTH) | (st.st_mode & S_ISUID) | (st.st_mode & S_ISGID) | (st.st_mode & S_ISVTX);
				COLOR_MAGENTA;
				printf("%-.4o ", perms);			// Print octal permissions
				COLOR_DEFAULT;

				// File type
				switch((st.st_mode >> 12) << 12) {
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

				// Permissions (human-readable)
				int perm = st.st_mode & 0007777;

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

				// File Size
				COLOR_RED;
				printf("%*llu ", size_length, (unsigned long long int) st.st_size);
				COLOR_DEFAULT;

				// Last Modifcation Date
				char buffer[16];
				strftime(buffer, 16, "%b %e %R", localtime(&st.st_mtime));
				COLOR_YELLOW;
				printf("%s ", buffer);
				COLOR_DEFAULT;
			}

			// File Name
			COLOR_OFF;
			if (args != NULL && HAS_ARG('l')) {		// If list mode
				printf("%s", dirp[i] -> d_name);
				if (((st.st_mode >> 12) << 12) == S_IFDIR && HAS_ARG('f')) {	// If labeling directories
					COLOR_BLUE;
					COLOR_BOLD;
					printf("/");
					COLOR_DEFAULT;
				}
				printf("\n");
			}
			else {									// If not list mode
				if (line_length + strlen(dirp[i] -> d_name) + 2 < term_w) {			// If output fits within terminal width
					printf("%s", dirp[i] -> d_name);
					if (((st.st_mode >> 12) << 12) == S_IFDIR && HAS_ARG('f')) {
						COLOR_BLUE;
						COLOR_BOLD;
						printf("/");
						COLOR_DEFAULT;
						line_length++;
					}
					line_length += strlen(dirp[i] -> d_name);
					printf("  ");
					line_length += 2;
				}
				else if (line_length + strlen(dirp[i] -> d_name) < term_w) {		// If output barely fits
					printf("%s", dirp[i] -> d_name);
					if (((st.st_mode >> 12) << 12) == S_IFDIR && HAS_ARG('f')) {
						COLOR_BLUE;
						COLOR_BOLD;
						printf("/");
						COLOR_DEFAULT;
						line_length++;
					}
					line_length += strlen(dirp[i] -> d_name);

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
			free(dirp[i]);
		}

		free(dirp);
		if (args == NULL || !HAS_ARG('l')) {	// If list mode
			printf("\n");
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
