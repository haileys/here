#ifndef HTTP_H
#define HTTP_H

typedef enum http_method {
    HTTP_GET,
    HTTP_HEAD,
    HTTP_POST,
} http_method_t;

typedef enum http_version {
    HTTP_1_0,
    HTTP_1_1,
} http_version_t;

typedef struct http_header_field {
    struct http_header_field* next;
    char* name;
    char* value;
} http_header_field_t;

typedef struct http_request_header {
    http_method_t method;
    char* uri;
    http_version_t version;
    http_header_field_t* fields;
} http_request_header_t;

void http_free_request_header(http_request_header_t* header);
int http_parse_headers(char* buff, int length, http_request_header_t** headers_out, int* status, char** error);
char* http_method_str(http_method_t method);
char* http_error_response(char* buff, int error_code, char* message);

#endif