#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <time.h>

char command[4];

bool equalStrings(char * stringA, char * stringB) {
    return strcmp(stringA, stringB) == 0;
}

void printPermissions(mode_t mode) {

    // print whether entry is a directory or a regular file
    printf("%c", (S_ISDIR(mode) ? 'd' : S_ISREG(mode) ? 'a' : '-'));

    // print owner permissions
    printf("%c%c%c",
        mode & S_IRUSR ? 'r' : '-',
        mode & S_IWUSR ? 'w' : '-',
        mode & S_IXUSR ? 'e' : '-'
    );

    // print group permissions
    printf("%c%c%c",
        mode & S_IRGRP ? 'r' : '-',
        mode & S_IWGRP ? 'w' : '-',
        mode & S_IXGRP ? 'e' : '-'
    );

    // print other permissions
    printf("%c%c%c",
        mode & S_IROTH ? 'r' : '-',
        mode & S_IWOTH ? 'w' : '-',
        mode & S_IXOTH ? 'e' : '-'
    );

}

// From infodemo.c
void getAndPrintGroup(gid_t grpNum) {
  struct group *grp;

  grp = getgrgid(grpNum); 
  
  if (grp) {
    printf("%s", grp->gr_name);
  }
}



void getAndPrintUserName(uid_t uid) {

  struct passwd *pw = NULL;
  pw = getpwuid(uid);

  if (pw) {
    printf("%s", pw->pw_name);
  } 
}

void ls(const char * directory, bool inode, bool longList, bool recursive) {
    DIR * dir;
    struct dirent *ptr;
    struct stat buf;
    dir = opendir(directory);

    int dir_length = strlen(directory);
    char * newPath = NULL;
    while((ptr = readdir(dir)) != NULL) {

        // Create new string that stores the path to new directory point
        // - May need to rework into a more elagant solution
        char * dirEntry = ptr->d_name;
        int newEntryLength = strlen(dirEntry);
        newPath = malloc(dir_length + 2 + newEntryLength);

        // Combining directory root with directory entry
        strcpy(newPath, directory);
        strcat(newPath, "/");
        strcat(newPath, dirEntry);


        if (stat(newPath, &buf) != -1) {
            // If -i has been set
            if(inode) {
                int inode = buf.st_ino;
                printf("%d ", inode);
            }
            // If -l has been set
            if(longList) {

                printPermissions(buf.st_mode);
                int numHardLinks = buf.st_nlink;
                int sizeInBytes = buf.st_size;
                
                // Extracted time from: https://www.cplusplus.com/reference/ctime/strftime/
                struct tm * timeInfo;
                char date[100];
                timeInfo = localtime(&buf.st_mtime);
                strftime(date, 100, "%b %d %Y %I:%M", timeInfo);

                printf(" %d ", numHardLinks);
                getAndPrintUserName(buf.st_uid);
                printf(" ");
                getAndPrintGroup(buf.st_gid);
                printf(" %d %s", sizeInBytes, date);
            }

            // Finally, print out the path of the new directory entry
            printf(" %s\n", newPath);
            
            // If -R has been set, and checking whether
            // - the current file is not . / .. to avoid segmentation faults
            // - the current entry is a directory
            if(recursive 
                && S_ISDIR(buf.st_mode) // Checks if the entry is a directory
                && (!equalStrings(dirEntry, "..") && !equalStrings(dirEntry, "."))) {
                ls(newPath, inode, longList, recursive);
            }
        }
        free(newPath);
    }
    closedir(dir);
}


void lsi(char * directory) {
    DIR * dir;
    struct dirent *ptr;
    struct stat buf;
    
    char *dir_str = ".";
    dir = opendir(dir_str);
    
    size_t dir_length = strlen(dir_str);
    char *path = malloc(dir_length + 1);
    strcpy(path, dir_str);
    path[dir_length] = '/';
    
    while((ptr = readdir(dir)) != NULL) {

        // Create new string that stores the path to new directory point
        // - May need to rework into a more elagant solution
        int newEntryLength = strlen(ptr->d_name);
        char * newPath = malloc(dir_length + 1 + newEntryLength);
        //strncpy(newPath, path, sizeof(path));
        strcat(newPath, ptr->d_name);
        printf("Path: %s\n", newPath);

        if (stat(newPath, &buf) != -1) {
        //    printf("%d bytes\n", (int)buf.st_size);
        //    printf("%ld\n", buf.st_atime);
        }
        free(newPath);
        
    }
    closedir(dir);
}

void lsr(char *dir_str) {
    DIR * dir;
    struct dirent *ptr;
    struct stat buf;
    
    dir = opendir(dir_str);
    
    size_t dir_length = strlen(dir_str);
    char *path = malloc(dir_length + 1 + NAME_MAX);
    strcpy(path, dir_str);
    path[dir_length] = '/';
    
    int i = 0;
    while((ptr = readdir(dir)) != NULL) {
        strcpy(path + dir_length + 1, ptr->d_name);
        if (stat(path, &buf) != -1) {
//            printf("%s\n", path);
            if (i>1) {
                printf("%s\n", ptr->d_name);
            }
        }
        if (i>1 && S_ISDIR(buf.st_mode)) {
            printf("going to %s\n", path);
            lsr(path);
        }
        i++;
    }
    closedir(dir);
}

// ls -i = list files with inode index
// ls -l = long listing format
// ls -R = list subdirectories recursively

int main(int argc, const char * argv[]) {
    // insert code here...
    // while (1) {
    //     printf("Hello, command?\n");
    //     printf("\n");
    //     scanf("%s", command);
        
    //     if (command[0] == 'l' && command[1] == 's') {
    //         ls();
    //     } else if (command[0] == 'i') {
    //         lsi();
    //     } else if (command[0] == 'R') {
    //         lsr(".");
    //     }
    //     printf("\n");
    // }
    
    // TODO: determine how to get options from the options list
    if(argc < 2) {
        printf("usage = ./UnixLs -{options} -{directory/directories}\n");
        return 0;
    }
    
    // Set booleans accordingly
    // 1. inode
    // 2. long listing
    // 3. recursive
    ls(argv[1], true, true, false);

    return 0;
}
