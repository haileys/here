#include <stdio.h>
#include <string.h>
#include <time.h>
#include "module.h"

int here_dispatch(server_t* server, client_t* client, http_request_header_t* headers, char* real_path, char* extension) {
    if(strcmp("/time", headers->uri)) {
        char buff[1000];
        sprintf(buff, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\nThe current UNIX timestamp is: <b>%d</b>", (int)time(NULL));
        send(client->sockfd, buff, strlen(buff), 0);
        return 1;
    }
    return 0;
}