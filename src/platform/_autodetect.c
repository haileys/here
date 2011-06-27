#ifdef __linux__
    #include "linux.c"
#endif
#if (defined(BSD) || defined(__APPLE__))
    #include "bsd.c"
#endif