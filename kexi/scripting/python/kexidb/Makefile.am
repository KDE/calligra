noinst_LTLIBRARIES = libkrosspythonkexidb.la

libkrosspythonkexidb_la_SOURCES = \
    pythonkexidb.cpp \
    pythonkexidbdriver.cpp \
    pythonkexidbconnectiondata.cpp pythonkexidbconnection.cpp \
    pythonkexidbcursor.cpp \
    pythonkexidbfield.cpp pythonkexidbfieldlist.cpp \
    pythonkexidbschema.cpp \
    pythonkexidbtransaction.cpp

libkrosspythonkexidb_la_LDFLAGS = $(PYTHON_FLAGS) $(all_libraries) -Wnounresolved
libkrosspythonkexidb_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    ../CXX/libkrosspythoncxx.la \
    $(top_builddir)/kexi/kexidb/libkexidb.la \
     $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la \
    $(PYTHON_LIB)

##../main/libkrosspythonmain.la

KDE_CXXFLAGS = $(USE_EXCEPTIONS)

libkrosspythonkexidb_la_METASOURCES = AUTO

SUBDIRS = .

INCLUDES = -I$(top_srcdir)/kexi $(PYTHON_INCLUDE) $(all_includes)

