noinst_LTLIBRARIES = libkrosspythoncxx.la

libkrosspythoncxx_la_SOURCES = \
    cxxsupport.cxx \
    cxx_extensions.cxx \
    cxxextensions.c \
    IndirectPythonInterface.cxx

libkrosspythoncxx_la_LDFLAGS = $(all_libraries) -Wnounresolved
libkrosspythoncxx_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    -l$(PYTHON_LIB)

libkrosspythoncxx_la_METASOURCES = AUTO

SUBDIRS = .

INCLUDES = \
    -I$(PYTHON_INCLUDE) \
    $(KEXI_INCLUDE) \
    $(all_includes)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)
#CXXFLAGS = $(USE_EXCEPTIONS)

clean:
	@rm -f *.o 2> /dev/null
	@rm -f $(BIN) 2> /dev/null
