include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkrossmain.la

libkrossmain_la_SOURCES = \
    scriptcontainer.cpp \
    eventcollection.cpp \
    manager.cpp

libkrossmain_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved

libkrossmain_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../api/libkrossapi.la

libkrossmain_la_METASOURCES = AUTO
SUBDIRS = .
INCLUDES = -I$(top_srcdir)/kexi $(all_includes)
