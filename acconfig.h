
#undef VERSION

#undef PACKAGE

#undef HAVE_BOOL

#undef ksize_t

#undef HAVE_MINI_STL

#undef ENABLE_NLS

#undef HAVE_QIMGIO

#undef HAVE_LIBZ

#undef HAVE_LIBJPEG

#undef HAVE_LIBGIF

#undef HAVE_LIBTIFF

#undef HAVE_LIBPNG

#undef WITH_SYMBOL_UNDERSCORE

#undef KDEMAXPATHLEN

#ifndef HAVE_BOOL
#define HAVE_BOOL
typedef int bool;
#ifdef __cplusplus
const bool false = 0;
const bool true = 1;
#else
#define false (bool)0;
#define true (bool)1;
#endif
#endif

