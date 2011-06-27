#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "util.h"

void fatal(char* fmt, ...) {
    va_list va;
    va_start(va, fmt);
    fprintf(stderr, "FATAL ");
    vfprintf(stderr, fmt, va);
    fprintf(stderr, "\n");
    exit(EXIT_FAILURE);
}