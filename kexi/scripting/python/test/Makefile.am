noinst_LTLIBRARIES = libkrosspythontest.la

libkrosspythontest_la_SOURCES = test.cpp
libkrosspythontest_la_LDFLAGS = $(all_libraries) -Wnounresolved

libkrosspythontest_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../CXX/libkrosspythoncxx.la \
    ../kexidb/libkrosspythonkexidb.la \
    -lkexidbparser -lkexidb \
    -l$(PYTHON_LIB)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libkrosspythontest_la_METASOURCES = AUTO

SUBDIRS = .

INCLUDES = \
    -I$(top_srcdir)/test \
    -I$(top_srcdir)/CXX \
    -I$(top_srcdir)/kexidb \
    -I$(PYTHON_INCLUDE) \
    $(KEXI_INCLUDE) \
    $(all_includes)

clean:
	@rm -f *.o 2> /dev/null
	@rm -f $(BIN) 2> /dev/null
