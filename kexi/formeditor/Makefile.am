INCLUDES = $(all_includes)
lib_LTLIBRARIES =  libkformeditor.la
libkformeditor_la_LDFLAGS = -no-undefined $(all_libraries)
libkformeditor_la_SOURCES = container.cpp resizehandle.cpp widgetfactory.cpp widgetlibrary.cpp libactionwidget.cpp form.cpp objecttree.cpp propertybuffer.cpp  propertybufferitem.cpp  propertyeditor.cpp  propertyeditoreditor.cpp  propertyeditorinput.cpp  propertyeditoritem.cpp  propertyeditorlist.cpp eventdetail.ui

METASOURCES = AUTO
libkformeditor_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KPARTS)

servicetypesdir = $(kde_servicetypesdir)
servicetypes_DATA=widgetfactory.desktop

SUBDIRS = . factories test

include $(top_srcdir)/kexi/Makefile.global
