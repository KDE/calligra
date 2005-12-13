include $(top_srcdir)/lib/kross/Makefile.global

lib_LTLIBRARIES = libkrossmain.la

libkrossmain_la_SOURCES = \
    mainmodule.cpp \
    scriptcontainer.cpp \
    manager.cpp \
    scriptaction.cpp \
    scriptguiclient.cpp

libkrossmain_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved

libkrossmain_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    $(LIB_KFILE) \
    $(LIB_KDEUI) \
    $(LIB_KROSS_API)

METASOURCES = AUTO
SUBDIRS = .
INCLUDES = $(KROSS_INCLUDES) $(all_includes)
