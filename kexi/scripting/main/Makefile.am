include $(top_srcdir)/kexi/Makefile.global

noinst_LTLIBRARIES = libkrossmain.la

libkrossmain_la_SOURCES = \
    scriptcontainer.cpp \
    manager.cpp

libkrossmain_la_LDFLAGS = $(all_libraries) -Wnounresolved
#$(PYTHON_FLAGS)

libkrossmain_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../api/libkrossapi.la \
    ../kjs/libkrosskjs.la \
    ../python/libkrosspython.la

libkrossmain_la_METASOURCES = AUTO
SUBDIRS = .
INCLUDES = -I$(top_srcdir)/kexi $(PYTHON_INCLUDE) $(all_includes)
