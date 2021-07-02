#define _GNU_SOURCE
#define __USE_GNU

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
static char *hide_process_name[] = { NAMES };
static int hide_GPU_Index[] = { INDEXS };

extern void *__libc_dlsym(void *, const char *);
