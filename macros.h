#pragma once

#include <alloca.h>
#include <sys/types.h>
#include <string.h>

#ifndef memdupa
#define memdupa(ptr, length, size) \
  __extension__ ({ size_t bytes = (length) * (size); \
                   memcpy (alloca (bytes), (ptr), bytes); })
#endif

#ifndef arraydupa
#define arraydupa(array, length) \
  memdupa ((array), (length), sizeof (array[0]))
#endif
