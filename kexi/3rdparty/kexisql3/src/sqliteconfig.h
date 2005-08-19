#ifdef _WIN32
# define SQLITE_PTR_SZ 4
#else
#ifndef _BKSYS
# include "../../../../config.h"
# define SQLITE_PTR_SZ SIZEOF_CHAR_P
#endif
#endif
