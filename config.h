#define _GNU_SOURCE
#define __USE_GNU

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
static const char *hide_process_name[] = {"Xorg", "alacritty"};
static const int hide_GPU_Index[] = {0,3};

extern void *__libc_dlsym(void *, const char *);
