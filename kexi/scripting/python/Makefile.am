noinst_LTLIBRARIES = libkrosspython.la

SUBDIRS = CXX .

libkrosspython_la_SOURCES = \
    pythonextension.cpp \
    pythonmodule.cpp \
    pythoninterpreter.cpp

libkrosspython_la_LDFLAGS = $(PYTHON_FLAGS) $(all_libraries) -Wnounresolved

libkrosspython_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    $(PYTHON_LIB) \
    CXX/libkrosspythoncxx.la \
    ../api/libkrossapi.la

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libkrosspython_la_METASOURCES = AUTO

INCLUDES = -I$(top_srcdir)/kexi $(PYTHON_INCLUDE) $(all_includes)
