#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <signal.h>
#include <unistd.h>
#ifdef __linux__
    #include <sys/sendfile.h>
    #include <linux/stat.h>
#endif
#include "handlers.h"
#include "platform.h"
#include "http.h"

void process_request(server_t* server, client_t* client)
{
    char buff[1024];
    int n = recv(client->sockfd, buff, 1024, 0);
    char out[1024];
    int status = 0;
    char* message = NULL;
    http_request_header_t* headers;
    http_parse_headers(buff, n, &headers, &status, &message);
    if(message != NULL) {
        http_error_response(out, status, message);
        send(client->sockfd, out, strlen(out), 0);
        http_free_request_header(headers);
        return;
    }
    printf("%s %s\n", http_method_str(headers->method), headers->uri);
    
    char uripath[strlen(server->dir) + strlen(headers->uri) + 1];
    sprintf(uripath, "%s%s", server->dir, headers->uri);
    char base[PATH_MAX + 1];
    struct stat buf;
    
    if(realpath(uripath, base) == NULL || strstr(base, server->dir) != base || stat(base, &buf) != 0) {
        http_error_response(out, 404, "Not Found");
        send(client->sockfd, out, strlen(out), 0);
        http_free_request_header(headers);
        return;
    }
    
    if(S_ISDIR(buf.st_mode)) {
        int base_len = strlen(base);
        if(base[base_len - 1] != '/') {
            base[base_len++] = '/';
            base[base_len] = 0;
        }
        for(int i = 0; i < server->index_files_length; i++) {
            char try[base_len + strlen(server->index_files[i]) + 1];
            sprintf(try, "%s%s", base, server->index_files[i]);
            if(stat(try, &buf) == 0) {
                dispatch_request(server, client, headers, try);
                http_free_request_header(headers);
                return;
            }
        }
        http_error_response(out, 404, "Not Found");
        send(client->sockfd, out, strlen(out), 0);
        http_free_request_header(headers);
        return;
    } else if(buf.st_mode & S_IFREG) {
        dispatch_request(server, client, headers, base);
        http_free_request_header(headers);
        return;
    } else {
        http_error_response(out, 404, "Not Found");
        send(client->sockfd, out, strlen(out), 0);
        http_free_request_header(headers);
        return;
    }
}

void dispatch_request(server_t* server, client_t* client, http_request_header_t* headers, char* real_path)
{
    char* extension = strrchr(strrchr(real_path, '/'), '.');
    
    for(int i = 0; i < server->filters_length; i++) {
        if(server->filters[i](server, client, headers, real_path, extension)) {
            return;
        }
    }
    
    char* mimetype = "application/x-octet-stream";
    if(extension != NULL) {
        if(strcmp(extension, ".html") == 0) {
            mimetype = "text/html";
        }
    }
    char buff[1000];
    FILE* f = fopen(real_path, "rb");
    if(f == NULL) {
        http_error_response(buff, 403, "Forbidden");
        send(client->sockfd, buff, strlen(buff), 0);
        return;
    }
    fseek(f, 0, SEEK_END);
    size_t length = ftell(f);
    fclose(f);
    sprintf(buff, "HTTP/1.1 200 OK\r\nContent-Type: %s\r\nContent-Length: %d\r\nConnection: close\r\nServer: here\r\n\r\n", mimetype, (int)length);
    send(client->sockfd, buff, strlen(buff), 0);
    int fd = open(real_path, O_RDONLY);
    platform_sendfile(client->sockfd, fd, length);
}
