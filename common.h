#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <errno.h>
#include <setjmp.h>
#include <signal.h>
#include <sys/types.h>

#define USAGE() { fprintf(stderr, "Usage: %s P Q taskid R\n", argv[0]); exit(123); }

// guard for syscall error
#define G(expr) if ((expr) < 0) { perror(#expr); exit(221); }

#define SIGUSR3 SIGWINCH
#define ACK "ACK"

#ifdef DEBUG
# define LOG(format, ...) fprintf(stderr, format "\n", ##__VA_ARGS__)
# define INFO(format, ...) fprintf(stderr, format "\n", ##__VA_ARGS__)
# define ONE_SEC 1
# define FIVE_SEC 5
#else
# define LOG(...)
# define INFO(...)
# define ONE_SEC 1
# define FIVE_SEC 5
#endif
