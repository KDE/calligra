INCLUDES = -I$(KOFFICE_SRCDIR)/kword $(all_includes)

KDE_CXXFLAGS = -frtti

lib_LTLIBRARIES = libkword_adresses.la

libkword_adresses_la_SOURCES = factory.cpp plugin.cpp

libkword_adresses_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN)

libkword_adresses_la_LIBADD = $(LIB_KDBACCESS) $(LIB_KPARTS)

libkword_adresses_la_METASOURCES = AUTO

ressourcedir = $(kde_datadir)/kword/kpartplugins
ressource_DATA = kword_adresses.rc
