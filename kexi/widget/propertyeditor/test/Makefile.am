include $(top_srcdir)/kexi/Makefile.global

INCLUDES = $(all_includes) -I$(top_srcdir)/kexi/core/properties

noinst_HEADERS = test.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

bin_PROGRAMS = test

test_SOURCES = main.cpp test.cpp
test_LDFLAGS = $(KDE_RPATH) $(all_libraries)
test_LDADD = $(LIB_KDEUI) ../libkexipropertyeditor.la 


# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/test
shellrc_DATA = testui.rc

test_INCLUDES= -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/core/properties -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi/widget/propertyeditor $(all_includes)

