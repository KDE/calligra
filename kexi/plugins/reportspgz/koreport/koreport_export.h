#ifndef KOREPORT_EXPORT_H
#define KOREPORT_EXPORT_H

// needed for KDE_EXPORT and KDE_IMPORT macros
#include <kdemacros.h>

#ifndef KOREPORT_EXPORT
# if defined(MAKE_KOREPORT_LIB)
// We are building this library
#  define KOREPORT_EXPORT KDE_EXPORT
# else
// We are using this library
#  define KOREPORT_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KOREPORT_EXPORT_DEPRECATED
#  define KOREORT_EXPORT_DEPRECATED KDE_DEPRECATED KOREPORT_EXPORT
# endif

#endif

