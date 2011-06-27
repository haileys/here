#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "http.h"

void http_free_request_header(http_request_header_t* header)
{
    free(header->uri);
    for(int i = 0; i < header->field_count; i++) {
        if(header->fields[i].name) {
            free(header->fields[i].name);
            free(header->fields[i].value);
        }
    }
    free(header->fields);
}

http_request_header_t* http_parse_headers(char* buff, size_t length, int* status, char** error)
{    
    char* method = buff;
    char* method_end = (char*)memchr(buff, ' ', length);
    if(method_end == NULL) {
        *status = 400;
        *error = "Bad Request";
        return NULL;
    }
    
    http_request_header_t* header = (http_request_header_t*)malloc(sizeof(http_request_header_t));
    
    length -= (method_end - buff) + 1;
    buff = method_end + 1;
    *method_end = 0;
    if(strcmp(method, "GET") == 0) {
        header->method = HTTP_GET;
    } else if(strcmp(method, "HEAD") == 0) {
        header->method = HTTP_HEAD;
    } else if(strcmp(method, "POST") == 0) {
        header->method = HTTP_POST;
    } else {
        *status = 405;
        *error = "Method Not Allowed";
        free(header);
        return NULL;
    }
    
    char* uri = buff;
    char* uri_end = (char*)memchr(buff, ' ', length);
    if(uri_end == NULL) {
        *status = 400;
        *error = "Bad Request";
        free(header);
        return NULL;
    }
    
    length -= (uri_end - buff) + 1;
    buff = uri_end + 1;
    *uri_end = 0;
    
    header->uri = (char*)malloc((uri_end - uri) + 1);
    memcpy(header->uri, uri, (uri_end - uri) + 1);
    
    
}

char* http_error_response(char* buff, int error_code, char* message)
{
    char body[1000];
    sprintf(body, "<h1>%d %s</h1><hr /><address>here webserver</address>", error_code, message);
    sprintf(buff, "HTTP/1.1 %d %s\r\nServer: here\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s",
        error_code, message, (int)strlen(body), body);
    return buff;
}