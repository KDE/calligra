include $(top_srcdir)/kexi/Makefile.global

noinst_LTLIBRARIES = libkrosskjs.la

libkrosskjs_la_SOURCES = \
    kjsinterpreter.cpp

libkrosskjs_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved

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

libkrosskjs_la_METASOURCES = AUTO
SUBDIRS = .
INCLUDES = -I$(top_srcdir)/kexi $(all_includes)
