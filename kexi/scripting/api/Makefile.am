noinst_LTLIBRARIES = libkrossapi.la

libkrossapi_la_SOURCES = \
    object.cpp \
    variant.cpp \
    list.cpp \
    argument.cpp \
    interpreter.cpp

#dict.cpp

libkrossapi_la_LDFLAGS = $(all_libraries) -Wnounresolved
libkrossapi_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
libkrossapi_la_METASOURCES = AUTO
SUBDIRS = .
INCLUDES = $(all_includes)
