include $(top_srcdir)/kexi/Makefile.global

lib_LTLIBRARIES = libkrossapi.la
#noinst_LTLIBRARIES = libkrossapi.la

libkrossapi_la_SOURCES = \
    object.cpp \
    variant.cpp \
    list.cpp \
    callable.cpp \
    argument.cpp \
    eventaction.cpp \
    eventsignal.cpp \
    eventslot.cpp \
    eventscript.cpp \
    qtobject.cpp \
    script.cpp \
    interpreter.cpp

#dict.cpp

libkrossapi_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
libkrossapi_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE)

libkrossapi_la_METASOURCES = AUTO
SUBDIRS = .
INCLUDES = -I$(top_srcdir)/kexi $(all_includes)
