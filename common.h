#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>

#define USAGE() { fprintf(stderr, "Usage: %s P Q taskid R\n", argv[0]); exit(123); }

#ifdef DEBUG
# define LOG(format, ...) fprintf(stderr, format "\n", ##__VA_ARGS__)
#else
# define LOG(...)
#endif
