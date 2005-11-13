include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi $(KROSS_INCLUDES) $(all_includes)

kde_module_LTLIBRARIES = krosskexidb.la

krosskexidb_la_SOURCES = \
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

krosskexidb_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
krosskexidb_la_LIBADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    $(LIB_KROSS_API) \
    $(LIB_KROSS_MAIN) \
    $(top_builddir)/kexi/kexidb/libkexidb.la \
    $(top_builddir)/kexi/kexidb/parser/libkexidbparser.la

METASOURCES = AUTO
SUBDIRS = .
