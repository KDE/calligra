noinst_LTLIBRARIES = libkrossmain.la

libkrossmain_la_SOURCES = \
    manager.cpp

libkrossmain_la_LDFLAGS = $(all_libraries) -Wnounresolved
#$(PYTHON_FLAGS)

libkrossmain_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../api/libkrossapi.la \
    ../kjs/libkrosskjs.la \
    ../python/libkrosspython.la

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
libkrossmain_la_METASOURCES = AUTO
SUBDIRS = .
INCLUDES = $(PYTHON_INCLUDE) $(all_includes)
