noinst_LTLIBRARIES = libkrosspythonmain.la

libkrosspythonmain_la_SOURCES = \
    pythonutils.cpp \
    pythonmanager.cpp

libkrosspythonmain_la_LDFLAGS = $(PYTHON_FLAGS) $(all_libraries) -Wnounresolved

libkrosspythonmain_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../CXX/libkrosspythoncxx.la \
    ../kexidb/libkrosspythonkexidb.la \
    $(top_builddir)/kexi/kexidb/libkexidb.la \
    $(PYTHON_LIB)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libkrosspythonmain_la_METASOURCES = AUTO

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/kexi $(PYTHON_INCLUDE) $(all_includes)

