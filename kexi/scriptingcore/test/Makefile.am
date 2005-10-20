include $(top_srcdir)/kexi/Makefile.global

noinst_PROGRAMS = krosstest

krosstest_SOURCES = testobject.cpp testaction.cpp testplugin.cpp testwindow.cpp main.cpp

krosstest_LDADD = \
    $(LIB_QT) \
    $(LIB_KDECORE) \
    $(LIB_KDEUI) \
    ../api/libkrossapi.la \
    ../main/libkrossmain.la

#../kexidb/libkrosskexidb.la
#../../kexiutils/libkexiutils.la

INCLUDES = -I$(top_srcdir)/kexi $(all_includes)
krosstest_LDFLAGS = $(all_libraries) $(KDE_RPATH)
SUBDIRS = .
METASOURCES = AUTO
