include $(top_srcdir)/kexi/Makefile.global

noinst_LTLIBRARIES = libkrosspython.la

libkrosspython_la_SOURCES = \
    pythonextension.cpp \
    pythonmodule.cpp \
    pythonobject.cpp \
    pythonscript.cpp \
    pythoninterpreter.cpp

libkrosspython_la_LDFLAGS = $(PYTHON_FLAGS) $(all_libraries) $(VER_INFO) -Wnounresolved

libkrosspython_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    $(PYTHON_LIB) \
    CXX/libkrosspythoncxx.la \
    ../api/libkrossapi.la

libkrosspython_la_METASOURCES = AUTO
SUBDIRS = CXX .
INCLUDES = -I$(top_srcdir)/kexi $(PYTHON_INCLUDE) $(all_includes)
