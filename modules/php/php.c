#include <stdio.h>
#include <string.h>
#include "module.h"
#include "heresapi.h"

void here_init()
{
    printf("Initializing PHP... ");
    fflush(stdout);
    if(herephp_initialize()) {
        printf("Ok\n");
    } else {
        printf("Failed!\n");
        exit(1);
    }
}

int here_dispatch(server_t* server, client_t* client, http_request_header_t* headers, char* real_path, char* extension)
{
    printf("%s\n", extension);
    return 0;
}