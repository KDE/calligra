include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi/widget/propertyeditor -I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi/tableview \
       -I$(top_srcdir)/kexi $(all_includes)
lib_LTLIBRARIES =  libkformdesigner.la
libkformdesigner_la_LDFLAGS = -no-undefined $(all_libraries)
libkformdesigner_la_SOURCES = container.cpp resizehandle.cpp widgetfactory.cpp objpropbuffer.cpp \
	 widgetlibrary.cpp libactionwidget.cpp form.cpp objecttree.cpp formIO.cpp formmanager.cpp \
	 objecttreeview.cpp spring.cpp commands.cpp events.cpp connectiondialog.cpp \
	 richtextdialog.cpp tabstopdialog.cpp editlistviewdialog.cpp utils.cpp

METASOURCES = AUTO
libkformdesigner_la_LIBADD = $(top_builddir)/kexi/widget/propertyeditor/libkexipropertyeditor.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la $(top_builddir)/kexi/tableview/libkexidatatable.la \
	$(LIB_KPARTS)

servicetypesdir = $(kde_servicetypesdir)
servicetypes_DATA=widgetfactory.desktop

SUBDIRS = . factories test #kdevelop_plugin

messages: rc.cpp
	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui"` >> rc.cpp
	$(XGETTEXT) `find . -name "*.cpp" -o -name ".h"` -o $(podir)/kformdesigner.pot

