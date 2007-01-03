include $(top_srcdir)/kexi/Makefile.global

all: transliteration_table.h

transliteration_table.h: transliteration_table.h.bz2
	bunzip2 -k transliteration_table.h.bz2

libkexiutils.la: transliteration_table.h
	

lib_LTLIBRARIES = libkexiutils.la
libkexiutils_la_SOURCES = identifier.cpp validator.cpp utils.cpp debuggui.cpp \
  styleproxy.cpp longlongvalidator.cpp transliteration_table.h

libkexiutilsincludedir=$(includedir)/kexiutils
libkexiutilsinclude_HEADERS = tristate.h identifier.h validator.h utils.h kexiutils_export.h \
	styleproxy.h longlongvalidator.h

noinst_HEADERS = utils_p.h

libkexiutils_la_LDFLAGS = \
  $(KDE_RPATH) $(all_libraries) \
  $(VER_INFO) -Wnounresolved -no-undefined

libkexiutils_la_LIBADD = $(LIB_QT) $(LIB_KDEUI) $(LIB_KIO) 

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)

METASOURCES = AUTO
