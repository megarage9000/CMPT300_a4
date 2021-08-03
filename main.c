#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>

char command[4];

void ls() {
    DIR * dir;
    struct dirent * ptr;
    
    dir = opendir(".");
    while((ptr = readdir(dir)) != NULL) {
        printf("%s\n", ptr->d_name);
    }
    closedir(dir);
}

int main(int argc, const char * argv[]) {
    // insert code here...
    while (1) {
        printf("Hello, command?\n");
        printf("\n");
        scanf("%s", command);
        
        if (command[0] == 'l' && command[1] == 's') {
            ls();
            
        }
        printf("\n");
    }
    
    return 0;
}
