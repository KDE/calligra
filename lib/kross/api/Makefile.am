include $(top_srcdir)/lib/kross/Makefile.global

lib_LTLIBRARIES = libkrossapi.la

libkrossapi_la_SOURCES = \
    object.cpp \
    variant.cpp \
    list.cpp \
    dict.cpp \
    exception.cpp \
    callable.cpp \
    argument.cpp \
    eventaction.cpp \
    eventsignal.cpp \
    eventslot.cpp \
    eventscript.cpp \
    qtobject.cpp \
    script.cpp \
    interpreter.cpp

libkrossapi_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
libkrossapi_la_LIBADD = $(LIB_QT) $(LIB_KDECORE)

METASOURCES = AUTO
SUBDIRS = .
INCLUDES = $(KROSS_INCLUDES) $(all_includes)
