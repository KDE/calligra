bin_PROGRAMS = krosspython

export PYTHON_INCLUDE="/usr/include/python2.3"
export PYTHON_LIB="python2.3"
#export KEXI_INCLUDE = -I/home/snoopy/cvs/kde/koffice/kexi -I/home/snoopy/cvs/kde/koffice/kexi/kexidb
export KEXI_INCLUDE = \
    -I$(top_srcdir)/../.. \
    -I$(top_srcdir)/../../kexidb

SUBDIRS = CXX kexidb test .

krosspython_SOURCES = \
    pythonutils.cpp

INCLUDES = \
    -I$(srcdir)/test \
    -I$(srcdir)/CXX \
    -I$(srcdir)/kexidb \
    -I$(PYTHON_INCLUDE) \
    $(KEXI_INCLUDE) \
    $(all_includes)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

krosspython_LDADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    CXX/libkrosspythoncxx.la \
    test/libkrosspythontest.la \
    -lkexidbparser -lkexidb \
    -l$(PYTHON_LIB)

#LDFLAGS = -Xlinker -export-dynamic -rdynamic $(all_libraries)
krosspython_LDFLAGS = $(all_libraries) $(KDE_RPATH)
AM_CPPFLAGS = $(all_includes)

METASOURCES = AUTO

clean:
	@rm -f *.o 2> /dev/null
	@rm -f $(BIN) 2> /dev/null
