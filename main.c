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
#include <unistd.h>

char command[4];

bool equalStrings(char * stringA, char * stringB) {
    return strcmp(stringA, stringB) == 0;
}

void printPermissions(mode_t mode) {

    // print whether entry is a directory or a regular file
    printf("%3c", (S_ISDIR(mode) ? 'd' : S_ISLNK(mode) ? 'l' : '-'));

    // print owner permissions
    printf("%c%c%c",
        mode & S_IRUSR ? 'r' : '-',
        mode & S_IWUSR ? 'w' : '-',
        mode & S_IXUSR ? 'x' : '-'
    );

    // print group permissions
    printf("%c%c%c",
        mode & S_IRGRP ? 'r' : '-',
        mode & S_IWGRP ? 'w' : '-',
        mode & S_IXGRP ? 'x' : '-'
    );

    // print other permissions
    printf("%c%c%c",
        mode & S_IROTH ? 'r' : '-',
        mode & S_IWOTH ? 'w' : '-',
        mode & S_IXOTH ? 'x' : '-'
    );

}

// From infodemo.c
void getAndPrintGroup(gid_t grpNum) {
  struct group *grp;

  grp = getgrgid(grpNum); 
  
  if (grp) {
    printf("%11s", grp->gr_name);
  }
}



void getAndPrintUserName(uid_t uid) {

  struct passwd *pw = NULL;
  pw = getpwuid(uid);

  if (pw) {
    printf("%11s", pw->pw_name);
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
        if (dirEntry[0] == '.') {
            continue;
        }
        int newEntryLength = strlen(dirEntry);
        newPath = malloc(dir_length + 2 + newEntryLength);
        
        // Combining directory root with directory entry
        strcpy(newPath, directory);
        strcat(newPath, "/");
        strcat(newPath, dirEntry);
        
        if (lstat(newPath, &buf) != -1) {
            // If -i has been set
            if(inode) {
                int inode = buf.st_ino;
                printf("%9d ", inode);
            }
            // If -l has been set
            if(longList) {

                printPermissions(buf.st_mode);
                int numHardLinks = buf.st_nlink;
                long sizeInBytes = buf.st_size;
                
                // Extracted time from: https://www.cplusplus.com/reference/ctime/strftime/
                struct tm * timeInfo;
                char date[20];
                
                timeInfo = localtime(&buf.st_mtime);
                strftime(date, 20, "%b %d %Y %H:%M", timeInfo);
    
                printf(" %6d ", numHardLinks);
                getAndPrintUserName(buf.st_uid);
                printf(" ");
                getAndPrintGroup(buf.st_gid);
                printf(" %11ld    %11s", sizeInBytes, date);
            }

            // Finally, print out the path of the new directory entry
//            printf("   %s", newPath);
            printf("   %s", ptr->d_name);
            if (longList && S_ISLNK(buf.st_mode)) {
                // https://www.ibm.com/docs/en/zos/2.4.0?topic=functions-readlink-read-value-symbolic-link
                char *tmp;
                tmp = malloc(10);
                tmp[9] = '\0';
                if (readlink(dirEntry, tmp, sizeof(tmp)) < 0) {
                  perror("readlink() error");
                } else {
                    printf(" -> %s", tmp);
                }
            }
            printf("\n");
            
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
    
    // https://www.delftstack.com/howto/c/string-contains-in-c/
    char *ret;
    bool l = false;
    bool i = false;
    bool r = false;
    int onlyOneArg = 3;
    
    ret = strstr(argv[1], "l");
    if (ret) {
        l = true;
        onlyOneArg--;
    } else if (argc == 4) {
        ret = strstr(argv[2], "l");
        if (ret) {
            l = true;
            onlyOneArg--;
        }
    }
    
    ret = strstr(argv[1], "i");
    if (ret) {
        i = true;
        onlyOneArg--;
    } else if (argc == 4) {
        ret = strstr(argv[2], "i");
        if (ret) {
            i = true;
            onlyOneArg--;
        }
    }
    
    ret = strstr(argv[1], "R");
    if (ret) {
        r = true;
        onlyOneArg--;
    } else if (argc == 4) {
        ret = strstr(argv[2], "R");
        if (ret) {
            r = true;
            onlyOneArg--;
        }
    }
    
    // Set booleans accordingly
    // 1. inode
    // 2. long listing
    // 3. recursive
    if (argc == 4) {
        ls(argv[3], i, l, r);
    } else if (onlyOneArg != 3) {
        ls(argv[2], i, l, r);
    } else {
        ls(argv[1], false, false, false);
    }
    

    return 0;
}
