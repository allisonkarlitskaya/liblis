#pragma once

#include <stdnoreturn.h>

noreturn void
__attribute__ ((format (printf, 1, 2)))
abort_with_message (const char *format,
                    ...);
