#include <koffice_export.h>

#ifndef _KWORD_EXPORT_H
#define _KWORD_EXPORT_H

#ifdef COMPILING_TESTS
# ifdef Q_WS_WIN
#   ifdef MAKE_KWORD_TEST_LIB
#       define KWORD_TEST_EXPORT KDE_EXPORT
#   elif KDE_MAKE_LIB
#       define KWORD_TEST_EXPORT KDE_IMPORT
#   else
#       define KWORD_TEST_EXPORT
#   endif
# else // not windows
#   define KWORD_TEST_EXPORT KDE_EXPORT
# endif
#else // not compiling tests
#   define KWORD_TEST_EXPORT
#endif

#endif
