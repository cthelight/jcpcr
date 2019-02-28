#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdlib.h>

extern char *optarg;
extern int optind;

char *comp_str;
int comp_str_size;

char *cmd_str;
int cmd_str_size;


void listdir(char *path, size_t size) {
    DIR *dir;
    struct dirent *entry;
    size_t len = strlen(path);

    if (!(dir = opendir(path))) {
        fprintf(stderr, "path not found: %s: %s\n",
                path, strerror(errno));
        return;
    }

    //puts(path);
    while ((entry = readdir(dir)) != NULL) {
        char *name = entry->d_name;
        if (entry->d_type == DT_DIR) {
            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;
            if (len + strlen(name) + 2 > size) {
                fprintf(stderr, "path too long: %s/%s\n", path, name);
            } else {
                path[len] = '/';
                strcpy(path + len + 1, name);
                listdir(path, size);
                path[len] = '\0';
            }
        } else {

            //printf("%s/%s\n", path, name);
            if(strstr(name, ".jar") != NULL){
                comp_str_size += strlen(path) + strlen(name) + 3;
                comp_str = (char *)realloc(comp_str, comp_str_size);
                strcat(comp_str, ":");
                strcat(comp_str, path);
                strcat(comp_str,"/");
                strcat(comp_str, name);
            }
            //printf("%s\n", comp_str);
        }
    }
    closedir(dir);
}


int use_dir(const char *dir_name){
    char path[1024];
    strcpy(path, dir_name);
    listdir(path, sizeof(path));
    return 0;
}

int main(int argc, char *argv[]){
    int opt;
    comp_str = (char *)malloc(2 * sizeof(char));
    comp_str_size = 2;
    comp_str[0] = '\0';
    strcat(comp_str, ".");

    cmd_str = (char *)malloc(12 * sizeof(char));
    cmd_str[0] = '\0';
    cmd_str_size = 12;
    strcat(cmd_str, "javac -cp \"");
    while((opt = getopt(argc, argv, "j:d:")) != -1){
        switch(opt){
            case 'j':
                printf("%s\n", optarg);
                break;
            case 'd':
                use_dir(optarg);
                break;
            case '?':
                printf("oops\n");
                break;
        }
        
    }
    cmd_str_size += strlen(comp_str) + 1;
    cmd_str = (char *)realloc(cmd_str, cmd_str_size);
    strcat(cmd_str, comp_str);
    strcat(cmd_str, "\"");
    for(; optind < argc; optind++){
        cmd_str_size += strlen(argv[optind]) + 1;
        cmd_str = (char *)realloc(cmd_str, cmd_str_size);
        strcat(cmd_str, " ");
        strcat(cmd_str, argv[optind]);
    }
    printf("%s\n", cmd_str);
    system(cmd_str);
    free(comp_str) ;
    free(cmd_str);
}