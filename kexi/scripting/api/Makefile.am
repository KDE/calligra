include $(top_srcdir)/kexi/Makefile.global

noinst_LTLIBRARIES = libkrossapi.la

libkrossapi_la_SOURCES = \
    object.cpp \
    variant.cpp \
    list.cpp \
    argument.cpp \
    qtobject.cpp \
    signalhandler.cpp \
    script.cpp \
    interpreter.cpp

#dict.cpp

libkrossapi_la_LDFLAGS = $(all_libraries) -Wnounresolved
libkrossapi_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE)

libkrossapi_la_METASOURCES = AUTO
SUBDIRS = .
INCLUDES = -I$(top_srcdir)/kexi $(all_includes)
