#include "closefrom.h"

#include <assert.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

static void
test_errno_assumptions (void)
{
  errno = 0;
  close (open ("/dev/null", O_RDONLY));
  assert (errno == 0);

  errno = 123;
  close (open ("/dev/null", O_RDONLY));
  assert (errno == 123);

  errno = 0;
  close (open ("/dev/doesnotexist", O_RDONLY));
  assert (errno == EBADF);

  errno = 0;
  open ("/dev/doesnotexist", O_RDONLY);
  close (open ("/dev/null", O_RDONLY));
  assert (errno == ENOENT);

  errno = 9999;
  close (-1);
  assert (errno != 9999);
  assert (errno == EBADF);
}

static void
test_closefrom (void)
{
  dup2 (0, 77);
  dup2 (0, 1023);
  dup2 (0, 1000);

  errno = 9999;

  closefrom_via_proc (3);

  assert (errno == 9999);
}

int
main (int argc, char **argv)
{
  test_errno_assumptions ();
  test_closefrom ();
#if 0
  g_test_init (&argc, &argv);

  g_test_add_func ("/liblis/closefrom", test_closefrom);

  return g_test_run ();
#endif
  return 0;
}
