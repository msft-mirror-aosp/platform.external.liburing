/* SPDX-License-Identifier: MIT */
// https://syzkaller.appspot.com/bug?id=1f2ecd7a23dba87e5ca3505ec44514a462cfe8c0
// autogenerated by syzkaller (https://github.com/google/syzkaller)

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <unistd.h>

#include "liburing.h"
#include "helpers.h"
#include "../src/syscall.h"

static bool write_file(const char* file, const char* what, ...)
{
  char buf[1024];
  va_list args;
  va_start(args, what);
  vsnprintf(buf, sizeof(buf), what, args);
  va_end(args);
  buf[sizeof(buf) - 1] = 0;
  int len = strlen(buf);
  int fd = open(file, O_WRONLY | O_CLOEXEC);
  if (fd == -1)
    return false;
  if (write(fd, buf, len) != len) {
    int err = errno;
    close(fd);
    errno = err;
    return false;
  }
  close(fd);
  return true;
}

static int inject_fault(int nth)
{
  int fd;
  fd = open("/proc/thread-self/fail-nth", O_RDWR);
  if (fd == -1)
    exit(1);
  char buf[16];
  sprintf(buf, "%d", nth + 1);
  if (write(fd, buf, strlen(buf)) != (ssize_t)strlen(buf))
    exit(1);
  return fd;
}

static int setup_fault(void)
{
  static struct {
    const char* file;
    const char* val;
    bool fatal;
  } files[] = {
      {"/sys/kernel/debug/failslab/ignore-gfp-wait", "N", true},
      {"/sys/kernel/debug/failslab/verbose", "0", false},
      {"/sys/kernel/debug/fail_futex/ignore-private", "N", false},
      {"/sys/kernel/debug/fail_page_alloc/verbose", "0", false},
      {"/sys/kernel/debug/fail_page_alloc/ignore-gfp-highmem", "N", false},
      {"/sys/kernel/debug/fail_page_alloc/ignore-gfp-wait", "N", false},
      {"/sys/kernel/debug/fail_page_alloc/min-order", "0", false},
  };
  unsigned i;
  for (i = 0; i < sizeof(files) / sizeof(files[0]); i++) {
    if (!write_file(files[i].file, files[i].val)) {
      if (files[i].fatal)
	return 1;
    }
  }
  return 0;
}

static uint64_t r[2] = {0xffffffffffffffff, 0xffffffffffffffff};

int main(int argc, char *argv[])
{
  if (argc > 1)
    return T_EXIT_SKIP;
  mmap((void *) 0x20000000ul, 0x1000000ul, 3ul, MAP_ANON|MAP_PRIVATE, -1, 0);
  if (setup_fault()) {
    printf("Test needs failslab/fail_futex/fail_page_alloc enabled, skipped\n");
    return T_EXIT_SKIP;
  }
  intptr_t res = 0;
  *(uint32_t*)0x20000000 = 0;
  *(uint32_t*)0x20000004 = 0;
  *(uint32_t*)0x20000008 = 0;
  *(uint32_t*)0x2000000c = 0;
  *(uint32_t*)0x20000010 = 0;
  *(uint32_t*)0x20000014 = 0;
  *(uint32_t*)0x20000018 = 0;
  *(uint32_t*)0x2000001c = 0;
  *(uint32_t*)0x20000020 = 0;
  *(uint32_t*)0x20000024 = 0;
  *(uint32_t*)0x20000028 = 0;
  *(uint32_t*)0x2000002c = 0;
  *(uint32_t*)0x20000030 = 0;
  *(uint32_t*)0x20000034 = 0;
  *(uint32_t*)0x20000038 = 0;
  *(uint32_t*)0x2000003c = 0;
  *(uint32_t*)0x20000040 = 0;
  *(uint32_t*)0x20000044 = 0;
  *(uint64_t*)0x20000048 = 0;
  *(uint32_t*)0x20000050 = 0;
  *(uint32_t*)0x20000054 = 0;
  *(uint32_t*)0x20000058 = 0;
  *(uint32_t*)0x2000005c = 0;
  *(uint32_t*)0x20000060 = 0;
  *(uint32_t*)0x20000064 = 0;
  *(uint32_t*)0x20000068 = 0;
  *(uint32_t*)0x2000006c = 0;
  *(uint64_t*)0x20000070 = 0;
  res = __sys_io_uring_setup(0x6a6, (struct io_uring_params *) 0x20000000ul);
  if (res != -1)
    r[0] = res;
  res = socket(0x11ul, 2ul, 0x300ul);
  if (res != -1)
    r[1] = res;
  *(uint32_t*)0x20000080 = r[1];
  inject_fault(1);
  __sys_io_uring_register(r[0], 2ul, (const void *) 0x20000080ul, 1ul);
  return T_EXIT_PASS;
}
