noinst_LTLIBRARIES = libkrosspythoncxx.la

libkrosspythoncxx_la_SOURCES = \
    cxxsupport.cxx \
    cxx_extensions.cxx \
    cxxextensions.c \
    IndirectPythonInterface.cxx

libkrosspythoncxx_la_LDFLAGS = $(PYTHON_FLAGS) $(all_libraries) -Wnounresolved
libkrosspythoncxx_la_LIBADD = $(PYTHON_LIB)

libkrosspythoncxx_la_METASOURCES = AUTO

SUBDIRS = .

INCLUDES = $(PYTHON_INCLUDE) $(all_includes)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

clean:
	@rm -f *.o 2> /dev/null
	@rm -f $(BIN) 2> /dev/null
