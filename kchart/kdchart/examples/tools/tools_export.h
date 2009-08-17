#ifndef TESTTOOLS_EXPORT_H
#define TESTTOOLS_EXPORT_H

#include <qglobal.h>

// mostly copied from kdchart_export.h, might be overkill though

#ifdef Q_OS_WIN
# ifdef TESTTOOLS_NODLL
#  undef TESTTOOLS_MAKEDLL
#  undef TESTTOOLS_DLL
# elif defined( TESTTOOLS_MAKEDLL )
#  ifdef TESTTOOLS_DLL
#   undef TESTTOOLS_DLL
#  endif
#  ifdef KDCHART_BUILD_TESTTOOLS_LIB
#   define TESTTOOLS_EXPORT Q_DECL_EXPORT
#  else
#   define TESTTOOLS_EXPORT Q_DECL_IMPORT
#  endif
# elif defined( TESTTOOLS_DLL )
#  define TESTTOOLS_EXPORT Q_DECL_IMPORT
# endif
#else
# undef TESTTOOLS_MAKEDLL
# undef TESTTOOLS_DLL
#endif

#ifndef TESTTOOLS_EXPORT
# ifdef QT_SHARED
#  define TESTTOOLS_EXPORT Q_DECL_EXPORT
# else
#  define TESTTOOLS_EXPORT
# endif
#endif

#endif // TESTTOOLS_EXPORT_H
