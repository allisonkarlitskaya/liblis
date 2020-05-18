/* signal- and after-fork()-safe function to remap file descriptors
 * according to a specified array.  All other file descriptors are
 * closed.
 *
 * Commonly used after fork() and before exec().
 */
#include "fdremap.h"

#include <fcntl.h>
#include <unistd.h>

#include "abortwithmessage.h"
#include "closefrom.h"
#include "macros.h"

void
fd_remap (const int *remap_fds,
          int        n_remap_fds)
{
  int *fds = arraydupa (remap_fds, n_remap_fds);

  /* we need to get all of the remap-fds to be numerically above
   * n_remap_fds in order to make sure that we don't overwrite them in
   * the middle of the dup2() loop below, and also avoid the case that
   * dup2() is a no-op (which could fail to clear the O_CLOEXEC flag,
   * for example).
   */
  for (int i = 0; i < n_remap_fds; i++)
    if (fds[i] != -1) /* -1 == no-op */
      if (fds[i] < n_remap_fds)
        {
          int new_fd = fcntl (fds[i], F_DUPFD, n_remap_fds); /* returns >= n_remap_fds */

          if (new_fd == -1)
            abort_with_message ("fcntl(%d, F_DUPFD) failed", fds[i]);

          fds[i] = new_fd;
        }

  /* now we can map the fds into their final spot */
  for (int i = 0; i < n_remap_fds; i++)
    if (fds[i] != -1) /* no-op */
      if (dup2 (fds[i], i) != i)
        abort_with_message ("dup2(%d, %d) failed", fds[i], i);

  /* close everything else */
  closefrom_via_proc (n_remap_fds);
}
