#ifdef __APPLE__
# ifdef __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# else
#  undef WORDS_BIGENDIAN
# endif
#else
/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
#cmakedefine WORDS_BIGENDIAN ${CMAKE_WORDS_BIGENDIAN}
#endif
