#include <stdio.h>
#include <string.h>
#include <time.h>
#include "module.h"

int here_dispatch(server_t* server, client_t* client, http_request_header_t* headers, char* real_path, char* extension) {
    if(strcmp("/time", headers->uri) == 0) {
        char buff[1000];
        char body[100];
        sprintf(body, "The current UNIX timestamp is: <b>%d</b>", (int)time(NULL));
        sprintf(buff, "HTTP/1.1 200 OK\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s", (int)strlen(body), body);
        send(client->sockfd, buff, strlen(buff), 0);
        return 1;
    }
    return 0;
}