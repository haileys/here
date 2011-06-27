#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include "net.h"
#include "util.h"

server_t* create_server(int port, char* dir, void(*handler)(server_t*, client_t*))
{
    server_t* server = (server_t*)malloc(sizeof(server_t));
    server->port = port;
    server->dir = dir;
    server->handler = handler;
    server->current = NULL;
    
    server->listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&server->addr, 0, sizeof(server->addr));
    server->addr.sin_family = AF_INET;
    server->addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server->addr.sin_port = htons(server->port);
    if(bind(server->listenfd, (struct sockaddr*)&server->addr, sizeof(server->addr)) == -1) {
        fatal("could not bind to *:%d: %s", server->port, strerror(errno));
    }
    if(listen(server->listenfd, 1024) == -1) {
        fatal("could not listen");
    }
    
    return server;
}

void free_server(server_t* server)
{
    if(server->current != NULL)
    {
        close(server->current->sockfd);
        free(server->current);
    }    
    shutdown(server->listenfd, SHUT_RDWR);
    close(server->listenfd);
    free(server);
}

void server_accept_loop(server_t* server)
{
    while(1) {
        client_t* client = (client_t*)malloc(sizeof(client_t));
        unsigned int len = sizeof(server->addr);
        client->sockfd = accept(server->listenfd, (struct sockaddr*)&server->addr, &len);
        server->current = client;
        server->handler(server, client);
        shutdown(client->sockfd, SHUT_RDWR);
        close(client->sockfd);
        server->current = NULL;
        free(client);
    }
}