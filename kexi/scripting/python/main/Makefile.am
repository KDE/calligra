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
    $(KEXI_LIB) \
    $(PYTHON_LIB)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libkrosspythonmain_la_METASOURCES = AUTO

SUBDIRS = .

INCLUDES = \
    -I$(top_srcdir)/CXX \
    -I$(top_srcdir)/kexidb \
    $(PYTHON_INCLUDE) \
    $(KEXI_INCLUDE) \
    $(all_includes)

clean:
	@rm -f *.o 2> /dev/null
	@rm -f $(BIN) 2> /dev/null
