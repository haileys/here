#include "platform.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>

void platform_sendfile(int out, int in, size_t length)
{
    off_t len = length;
    sendfile(in, out, 0, &len, NULL, 0);
}