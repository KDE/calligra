noinst_LTLIBRARIES = libkrosskjs.la

libkrosskjs_la_SOURCES = \
    kjsinterpreter.cpp

libkrosskjs_la_LDFLAGS = $(all_libraries) -Wnounresolved

libkrosskjs_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../api/libkrossapi.la

#-lkjsembed
#/usr/lib/libkjsembed.la
#-l/usr/lib/python2.3/site-packages/qt.so
#$(LIB_KDEUI)
#$(LIB_KPARTS)
#$(LIB_KIO)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libkrosskjs_la_METASOURCES = AUTO

SUBDIRS = .

INCLUDES = $(all_includes)
#-I$(top_srcdir)/kexi
#$(KOFFICE_INCLUDES)
