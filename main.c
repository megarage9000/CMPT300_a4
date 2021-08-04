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

char command[4];

bool equalStrings(char * stringA, char * stringB) {
    return strcmp(stringA, stringB) == 0;
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

        // TODO: determine when to use lstat() for soft links
        if (stat(newPath, &buf) != -1) {
            // If -i has been set
            if(inode) {
                int inode = buf.st_ino;
                printf("%d ", inode);
            }
            // If -l has been set
            if(longList) {

            }

            // Finally, print out the path of the new directory entry
            printf("%s\n", newPath);
            
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

            
           printf("%d bytes\n", (int)buf.st_size);
           printf("%ld\n", buf.st_atime);
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
    // Assumes i, R is set
    // - TODO: implement long listing
    ls(argv[1], true, false, true);

    return 0;
}
