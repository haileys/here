#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <limits.h>
#include "util.h"
#include "net.h"
#include "http.h"

static server_t* server;

static void connection_handler(server_t* s, client_t* client)
{
    char buff[1024];
    int n = recv(client->sockfd, buff, 1024, 0);
    char out[1024];
    int status = 0;
    char* message = NULL;
    http_request_header_t* headers;
    int offset = http_parse_headers(buff, n, &headers, &status, &message);
    if(message != NULL) {
        http_error_response(out, status, message);
        send(client->sockfd, out, strlen(out), 0);
        return;
    }
    printf(">> %s\n", headers->uri);
    char msg[1000] = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nConnection: close\r\n\r\n";
    send(client->sockfd, msg, strlen(msg), 0);
    for(http_header_field_t* f = headers->fields; f != NULL; f = f->next) {
        sprintf(msg, "<strong>%s</strong> => %s<br />", f->name, f->value);
        send(client->sockfd, msg, strlen(msg), 0);
    }
    http_free_request_header(headers);
}

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
    char dir[PATH_MAX] = "./";
    
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
        fatal("unrecognized argument: %s", argv[i]);
    }
    
    signal(SIGINT, sigint);
    server = create_server(port, dir, connection_handler);
    server_accept_loop(server);
}