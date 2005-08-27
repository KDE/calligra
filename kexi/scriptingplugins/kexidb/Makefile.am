include $(top_srcdir)/kexi/Makefile.global

noinst_LTLIBRARIES = libkrosskexidb.la

libkrosskexidb_la_SOURCES = \
    kexidbfield.cpp \
    kexidbfieldlist.cpp \
    kexidbschema.cpp \
    kexidbparser.cpp \
    kexidbcursor.cpp \
    kexidbtransaction.cpp \
    kexidbconnectiondata.cpp \
    kexidbconnection.cpp \
    kexidbdriver.cpp \
    kexidbdrivermanager.cpp \
    kexidbmodule.cpp

libkrosskexidb_la_LDFLAGS = $(all_libraries) -Wnounresolved
libkrosskexidb_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    $(top_builddir)/kexi/scriptingcore/api/libkrossapi.la \
    $(top_builddir)/kexi/kexidb/libkexidb.la \
    $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la

libkrosskexidb_la_METASOURCES = AUTO
SUBDIRS = .
INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/scriptingcore $(all_includes)
