include $(top_srcdir)/kexi/Makefile.global

INCLUDES = $(all_includes) -I$(top_srcdir)/kexi/widget/propertyeditor
lib_LTLIBRARIES =  libkformeditor.la 
libkformeditor_la_LDFLAGS = -no-undefined $(all_libraries)
libkformeditor_la_SOURCES = container.cpp resizehandle.cpp widgetfactory.cpp objpropbuffer.cpp \
	 widgetlibrary.cpp libactionwidget.cpp form.cpp objecttree.cpp

METASOURCES = AUTO
libkformeditor_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KPARTS) $(top_srcdir)/kexi/widget/propertyeditor/libkexipropertyeditor.la

servicetypesdir = $(kde_servicetypesdir)
servicetypes_DATA=widgetfactory.desktop

SUBDIRS = . factories test

