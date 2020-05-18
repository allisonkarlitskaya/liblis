/* signal- and post-fork()-safe code to close all file descriptors above
 * a certain number.
 *
 * Commonly used after fork() and before exec().
 */

#define _GNU_SOURCE

#include "closefrom.h"

#include <dirent.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#include "abortwithmessage.h"

#ifndef PROC_SELF_FD
#define PROC_SELF_FD "/proc/self/fd"
#endif

void
closefrom_via_proc (int first_fd)
{
  ssize_t bytes_read;

  int proc_self_fd = open (PROC_SELF_FD, O_RDONLY | O_DIRECTORY);
  if (proc_self_fd == -1)
    abort_with_message ("failed to close all file descriptors: open('/proc/self/fd'): %m");

  do
    {
      char buffer[1024];
      ssize_t ofs = 0;

      /* getdents(2):
       *
       * Glibc does not provide a wrapper for these system calls; call them
       * using syscall(2).  You will need to define the linux_dirent or
       * linux_dirent64 structure yourself.
       */
      struct linux_dirent64 {
        ino64_t        d_ino;    /* 64-bit inode number */
        off64_t        d_off;    /* 64-bit offset to next structure */
        unsigned short d_reclen; /* Size of this dirent */
        unsigned char  d_type;   /* File type */
        char           d_name[]; /* Filename (null-terminated) */
      };

      bytes_read = syscall (SYS_getdents64, proc_self_fd, buffer, sizeof buffer);
      if (bytes_read < 0)
        abort_with_message ("failed to close all file descriptors: getdents64('/proc/self/fd'): %m");

      while (ofs < bytes_read)
        {
          const struct linux_dirent64 *de64 = (void *) &buffer[ofs];

          if (de64->d_type == DT_LNK)
            {
              long candidate_fd;
              char *end;

              candidate_fd = strtol (de64->d_name, &end, 10);

              if (*end == '\0' &&
                  first_fd <= candidate_fd && candidate_fd <= INT_MAX &&
                  candidate_fd != proc_self_fd)
                {
                  /* close() on Linux can return an error because of a
                   * previous failure (like failure to flush buffers on
                   * a socket) but regardless of any returned error, the
                   * fd is always actually closed.
                   *
                   * Ignore the return value.
                   */
                  (void) close (candidate_fd);
                }
            }

          ofs += de64->d_reclen;
        }
    }
  while (bytes_read > 0);

  close (proc_self_fd);
}
