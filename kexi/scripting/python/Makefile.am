bin_PROGRAMS = krosspython

export KEXI_INCLUDE = \
    -I$(top_srcdir)/../.. \
    -I$(top_srcdir)/../../kexidb

export KEXI_LIB = \
    -lkexidbparser \
    -lkexidb

SUBDIRS = CXX kexidb test .

krosspython_SOURCES = \
    pythonutils.cpp

INCLUDES = \
    -I$(srcdir)/test \
    -I$(srcdir)/CXX \
    -I$(srcdir)/kexidb \
    $(PYTHON_INCLUDE) \
    $(KEXI_INCLUDE) \
    $(all_includes)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

krosspython_LDADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    CXX/libkrosspythoncxx.la \
    test/libkrosspythontest.la \
    $(KEXI_LIB) \
    $(PYTHON_LIB)

#LDFLAGS = -Xlinker -export-dynamic -rdynamic $(all_libraries)
krosspython_LDFLAGS = ${PYTHON_FLAGS} $(all_libraries) $(KDE_RPATH)
AM_CPPFLAGS = $(all_includes)

METASOURCES = AUTO

clean:
	@rm -f *.o 2> /dev/null
	@rm -f $(BIN) 2> /dev/null
