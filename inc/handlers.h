#ifndef HANDLERS_H
#define HANDLERS_H

#include "net.h"

void process_request(server_t* server, client_t* client);
void dispatch_request(server_t* server, client_t* client, char* real_path);

#endif