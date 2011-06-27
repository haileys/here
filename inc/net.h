#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct client {    
    int sockfd;
    struct sockaddr_in addr;
} client_t;

typedef struct server {
    int listenfd;
    struct sockaddr_in addr;
    
    int port;
    char* dir;
    client_t* current;
    void(*handler)(struct server*, client_t*);
} server_t;

server_t* create_server(int port, char* dir, void(*handler)(server_t*, client_t*));
void free_server(server_t* server);

void server_accept_loop(server_t* server);

#endif