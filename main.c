#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <limits.h>
#include <string.h>
#include <grp.h>
#include <pwd.h>

char command[4];

void ls() {
    DIR * dir;
    struct dirent * ptr;
    int i = 0;
    dir = opendir(".");
    while((ptr = readdir(dir)) != NULL) {
        if (i>2) {
            printf("%s\n", ptr->d_name);
        }
        i++;
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
        int newEntryLength = strlen(ptr->d_name);
        char * newPath = malloc(dir_length + 1 + newEntryLength);
        strncpy(newPath, path, sizeof(path));
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
    
    if(argc < 2) {
        printf("usage = ./UnixLs -{options} -{directory/directories}\n");
        return 0;
    }
    lsi(argv[1]);

    return 0;
}
