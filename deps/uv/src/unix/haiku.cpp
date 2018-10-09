extern "C" {
#include "uv.h"
#include "internal.h"
}

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/resource.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>

#include <Directory.h>
#include <Entry.h>
#include <image.h>
#include <OS.h>
#include <Path.h>
#include <String.h>

#include <private/shared/cpu_type.h>

static void* args_mem = NULL;
static char** process_argv = NULL;
static int process_argc = 0;
static char* process_title_ptr = NULL;

int uv_exepath(char* buffer, size_t* size) {
  const char* str;
  image_info info;
  int32 cookie = 0;
  size_t strSize;

  if (buffer == NULL || size == NULL || *size == 0)
    return -EINVAL;

  while (get_next_image_info(0, &cookie, &info) == B_OK) {
    if (info.type == B_APP_IMAGE) {
      break;
    }
  }

  BEntry entry(info.name, true);
  BPath path;
  status_t rc = entry.GetPath(&path);  /* (path) now has binary's path. */
  if (rc != B_OK)
    return -errno;
  rc = path.GetParent(&path); /* chop filename, keep directory. */
  if (rc != B_OK)
    return -errno;
  str = path.Path();

  strSize -= 1;

 *size -= 1;

  strSize = sizeof str;
  if (*size > strSize)
    *size = strSize;

  memcpy(buffer, str, *size);
  buffer[*size] = '\0';

  return 0;
}

uint64_t uv_get_free_memory(void) {
  uint64 free_memory;

  system_info info;
  get_system_info(&info);

  free_memory = (info.free_memory) * B_PAGE_SIZE;

  return (uint64_t)free_memory;
}


uint64_t uv_get_total_memory(void) {
  uint64 total_memory;

  system_info info;
  get_system_info(&info);

  total_memory = (info.max_pages + info.ignored_pages) * B_PAGE_SIZE;

  return (uint64_t)total_memory;
}


void uv_loadavg(double avg[3]) {
  //Does not exist on Haiku...
  avg[0] = avg[1] = avg[2] = 0;
}

