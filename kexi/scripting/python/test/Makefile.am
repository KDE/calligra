noinst_PROGRAMS = krosspythontest

krosspythontest_SOURCES = test.cpp
    
krosspythontest_LDADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../CXX/libkrosspythoncxx.la \
    ../kexidb/libkrosspythonkexidb.la \
    ../main/libkrosspythonmain.la \
    $(top_builddir)/kexi/kexidb/libkexidb.la \
    $(PYTHON_LIB)
 
INCLUDES = -I$(top_srcdir)/kexi $(PYTHON_INCLUDE) $(all_includes)
    
KDE_CXXFLAGS = $(USE_EXCEPTIONS)

krosspythontest_LDFLAGS = ${PYTHON_FLAGS} $(all_libraries) $(KDE_RPATH)
#AM_CPPFLAGS = $(all_includes)

SUBDIRS = .

METASOURCES = AUTO

