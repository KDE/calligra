// Check windows
#ifdef Q_OS_WIN
   #ifdef _WIN64
     #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif

// Check GCC
#if __GNUC__
  #if defined (__x86_64__) || defined (__ppc64__)
    #define ENV64BIT
  #else
    #define ENV32BIT
  #endif
#endif


/* Number of bits in a file offset, on hosts where this is settable. */
#define _FILE_OFFSET_BITS 64

/* Define to 1 to make fseeko visible on some hosts (e.g. glibc 2.2). */
/* #undef _LARGEFILE_SOURCE */

/* Define for large files, on AIX-style hosts. */
/* #undef _LARGE_FILES */

/* Defines if we use KActivities */
#cmakedefine HAVE_KACTIVITIES 1

/* Defines if the old plugin metadata for mimetypes is used */
#cmakedefine CALLIGRA_OLD_PLUGIN_METADATA 1

/* Defines if KCalCore version with QDateTime-based API is available */
#cmakedefine HAVE_QDATETIME_KCALCORE 1

/* Defines Plan version */
#cmakedefine PLAN_VERSION_STRING "${PLAN_VERSION_STRING}"

/* Defines Plan year */
#cmakedefine PLAN_YEAR "${PLAN_YEAR}"

/* Defines use of KReport */
#cmakedefine PLAN_USE_KREPORT "${PLAN_USE_KREPORT}"
