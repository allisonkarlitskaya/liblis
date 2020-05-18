/* signal- and after-fork()-safe function to format a string, print it
 * to stderr and abort execution.  Never returns.
 */
#include "abortwithmessage.h"

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

noreturn void
__attribute__ ((format (printf, 1, 2)))
abort_with_message (const char *format,
                    ...)
{
  char buffer[1024];
  va_list ap;

  va_start (ap, format);
  size_t length = vsnprintf (buffer, sizeof buffer, format, ap);
  va_end (ap);

  /* Best effort */
  (void) write (STDERR_FILENO, buffer, length);

  abort ();
}
