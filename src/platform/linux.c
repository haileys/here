#include "platform.h"
#include <sys/types.h>

void platform_sendfile(int out, int in, size_t length)
{
    sendfile(out, in, NULL, length);
}