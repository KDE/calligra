include $(top_srcdir)/kexi/Makefile.global

INCLUDES = -I$(top_srcdir)/kexi/widget/propertyeditor -I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi/tableview \
       -I$(top_srcdir)/kexi $(all_includes)
lib_LTLIBRARIES =  libkformeditor.la
libkformeditor_la_LDFLAGS = -no-undefined $(all_libraries)
libkformeditor_la_SOURCES = container.cpp resizehandle.cpp widgetfactory.cpp objpropbuffer.cpp \
	 widgetlibrary.cpp libactionwidget.cpp form.cpp objecttree.cpp formIO.cpp formmanager.cpp \
	 objecttreeview.cpp spacer.cpp commands.cpp extrawidgets.cpp events.cpp

METASOURCES = AUTO
libkformeditor_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KPARTS) $(LIB_KIO) $(LIB_KDEFX) $(top_builddir)/kexi/widget/propertyeditor/libkexipropertyeditor.la \
	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la $(top_builddir)/kexi/tableview/libkexidatatable.la

servicetypesdir = $(kde_servicetypesdir)
servicetypes_DATA=widgetfactory.desktop

SUBDIRS = . factories test

