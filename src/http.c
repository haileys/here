#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "http.h"

static void rec_free_field(http_header_field_t* f) {
    if(f == NULL)
        return;
    rec_free_field(f->next);
    free(f->name);
    free(f->value);
    free(f);
}

void http_free_request_header(http_request_header_t* header)
{
    free(header->uri);
    rec_free_field(header->fields);
    free(header);
}

char* http_method_str(http_method_t method)
{
    switch(method)
    {
        case HTTP_GET: return "GET";
        case HTTP_POST: return "POST";
        case HTTP_HEAD: return "HEAD";
        default:
            return "?";
    }
}

enum parser_state
{
    P_NAME,
    P_VALUE,
    P_CRLF,
};

int http_parse_headers(char* buff, int length, http_request_header_t** headers_out, int* status, char** error)
{
    int original_length = length;
    
    char* method = buff;
    char* method_end = (char*)memchr(buff, ' ', length);
    if(method_end == NULL) {
        *status = 400;
        *error = "Bad Request";
        return -1;
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
        return -1;
    }
    
    char* uri = buff;
    char* uri_end = (char*)memchr(buff, ' ', length);
    if(uri_end == NULL) {
        *status = 400;
        *error = "Bad Request";
        free(header);
        return -1;
    }
    
    length -= (uri_end - buff) + 1;
    buff = uri_end + 1;
    *uri_end = 0;
    
    header->uri = (char*)malloc((uri_end - uri) + 1);
    memcpy(header->uri, uri, (uri_end - uri) + 1);
    
    // we just ignore the version because whatevs
    char* field_begin = memchr(buff, '\r', length);
    if(field_begin == NULL || buff + length <= field_begin + 4 || field_begin[1] != '\n') {
        *status = 400;
        *error = "Bad Request";
        free(header->uri);
        free(header);
        return -1;
    }
    field_begin += 2;
    length -= (field_begin - buff);
    char field_name[100];
    int fn_i = 0;
    char field_value[1000];
    int fv_i = 0;
    int state = P_CRLF;
    header->fields = NULL;
    http_header_field_t** next = &header->fields;
    int i;
    for(i = 0; i < length; i++) {
        if(buff[i] == '\r' && i + 1 < length && buff[i + 1] == '\n') {
            if(state == P_CRLF) {
                break;
            } else if(state == P_VALUE) {
                field_value[fv_i] = 0;
                *next = (http_header_field_t*)malloc(sizeof(http_header_field_t));
                (*next)->next = NULL;
                (*next)->name = (char*)malloc(strlen(field_name) + 1);
                strcpy((*next)->name, field_name);
                (*next)->value = (char*)malloc(strlen(field_value) + 1);
                strcpy((*next)->value, field_value);
                next = &(*next)->next;
            }
            state = P_CRLF;
            fn_i = 0;
            fv_i = 0;
            i++;
            continue;
        }
        if(buff[i] == ':' && i + 1 < length && buff[i + 1] == ' ') {
            if(state == P_CRLF) {
                http_free_request_header(header);
                *status = 400;
                *error = "Bad Request";
                return -1;
            } else if(state == P_NAME) {
                field_name[fn_i] = 0;
                state = P_VALUE;
                i++;
                continue;
            }
        }
        if(state == P_CRLF)
            state = P_NAME;
        if(state == P_NAME) {
            if(fn_i < 100) {
                field_name[fn_i++] = buff[i];
            }
        }
        if(state == P_VALUE) {
            if(fv_i < 1000) {
                field_value[fv_i++] = buff[i];
            }
        }
    }
    
    *headers_out = header;
    return i + (length - original_length);
}

char* http_error_response(char* buff, int error_code, char* message)
{
    char body[1000];
    sprintf(body, "<html><head><title>%s</title></head><body><h1>%d %s</h1><hr /><address>here webserver</address></body></html>", message, error_code, message);
    sprintf(buff, "HTTP/1.1 %d %s\r\nServer: here\r\nConnection: close\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n%s",
        error_code, message, (int)strlen(body), body);
    return buff;
}