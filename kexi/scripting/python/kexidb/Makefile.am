noinst_LTLIBRARIES = libkrosspythonkexidb.la

libkrosspythonkexidb_la_SOURCES = \
    pythonkexidb.cpp \
    pythonkexidbdriver.cpp \
    pythonkexidbconnectiondata.cpp pythonkexidbconnection.cpp \
    pythonkexidbcursor.cpp \
    pythonkexidbfield.cpp pythonkexidbfieldlist.cpp

libkrosspythonkexidb_la_LDFLAGS = ${PYTHON_FLAGS} $(all_libraries) -Wnounresolved
libkrosspythonkexidb_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../CXX/libkrosspythoncxx.la \
    $(KEXI_LIB) \
    $(PYTHON_LIB)

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libkrosspythonkexidb_la_METASOURCES = AUTO

SUBDIRS = .

INCLUDES = \
    -I$(top_srcdir)/CXX \
    -I$(top_srcdir)/test \
    -I$(top_srcdir)/kexidb \
    $(KEXI_INCLUDE) \
    $(PYTHON_INCLUDE) \
    $(all_includes)

clean:
	@rm -f *.o 2> /dev/null
	@rm -f $(BIN) 2> /dev/null
