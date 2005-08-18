include ../Makefile.global

lib_LTLIBRARIES = libshowimgkexiutils.la
libshowimgkexiutils_la_SOURCES = identifier.cpp validator.cpp utils.cpp

#libshowimgkexiutilsinclude_HEADERS = tristate.h identifier.h validator.h utils.h kexiutils_export.h

#noinst_HEADERS = utils_p.h

libshowimgkexiutils_la_LDFLAGS = \
  $(KDE_RPATH) $(all_libraries) \
  $(VER_INFO) -Wnounresolved -no-undefined

libshowimgkexiutils_la_LIBADD = $(LIB_QT) $(LIB_KDEUI) $(LIB_KIO) 

INCLUDES = -I$(showimg_top_srcdir)/kexi $(all_includes)

libshowimgkexiutils_la_METASOURCES = AUTO
