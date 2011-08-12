#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <dlfcn.h>
#include <signal.h>
#include <limits.h>
#include "handlers.h"
#include "util.h"
#include "net.h"

static server_t* server;

static void sigint(int n)
{    
    printf("Shutting down... ");
    free_server(server);
    printf("Bye.\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char** argv)
{
    int port = 8080;
    char dir[PATH_MAX] = ".";
    
    char** index_files = malloc(sizeof(char*) * 4);
    int index_files_capacity = 4;
    int index_files_length = 0;
    
    filter_t* filters = malloc(sizeof(filter_t) * 4);
    int filters_capacity = 4;
    int filters_length = 0;
    
    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--port") == 0) {
            if(i + 1 == argc)
                fatal("expected port number after -p or --port");
            i++;
            port = atoi(argv[i]);
            if(port < 1 || port > 65535)
                fatal("port not in range 1 - 65535");
            continue;
        }
        if(strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--dir") == 0) {
            if(i + 1 == argc)
                fatal("expected directory name after -d or --dir");
            i++;
            if(realpath(argv[i], dir) == NULL)
                fatal("dir %s: %s", argv[i], strerror(errno));
            continue;
        }
        if(strcmp(argv[i], "-i") == 0 || strcmp(argv[i], "--index") == 0) {
            if(i + 1 == argc)
                fatal("expected file name after -i or --index");
            i++;
            if(index_files_length == index_files_capacity) {
                index_files = realloc(index_files, index_files_capacity *= 2);
            }
            index_files[index_files_length] = malloc(strlen(argv[i]) + 1);
            strcpy(index_files[index_files_length], argv[i]);
            index_files_length++;
            continue;
        }
        if(strcmp(argv[i], "-m") == 0 || strcmp(argv[i], "--module") == 0) {
            if(i + 1 == argc)
                fatal("expected module name after -i or --index");
            i++;
            char mod_name[strlen(argv[i]) + 20];
            sprintf(mod_name, "here%s.so", argv[i]);
            void* mod_handle = dlopen(mod_name, RTLD_NOW | RTLD_LOCAL);
            char* err = dlerror();
            if(err) {
                fatal("couldn't load module %s: %s", argv[i], err);
            }
            void(*mod_init_fn)() = (void(*)())(size_t)dlsym(mod_handle, "here_init");
            filter_t mod_dispatch_fn = (filter_t)(size_t)dlsym(mod_handle, "here_dispatch");
            if(mod_dispatch_fn == NULL) {
                fatal("couldn't load module %s: no symbol here_dispatch", mod_name);
            }
            if(mod_init_fn != NULL) {
                mod_init_fn();
            }
            if(filters_length == filters_capacity) {
                index_files = realloc(filters, filters_capacity *= 2);
            }
            filters[filters_length++] = mod_dispatch_fn;
            continue;
        }
        fatal("unrecognized argument: %s", argv[i]);
    }
    
    signal(SIGINT, sigint);
    server = create_server(port, dir, index_files, index_files_length, filters, filters_length, process_request);
    server_accept_loop(server);
}