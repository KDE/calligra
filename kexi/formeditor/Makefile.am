INCLUDES = $(all_includes)
lib_LTLIBRARIES =  libkformeditor.la
libkformeditor_la_LDFLAGS = -no-undefined $(all_libraries)
libkformeditor_la_SOURCES = container.cpp container.h resizehandle.cpp resizehandle.h widgetfactory.cpp widgetfactory.h widgetlibrary.cpp widgetlibrary.h libactionwidget.cpp form.h form.cpp objecttree.cpp propertybuffer.cpp  propertybufferitem.cpp  propertyeditor.cpp  propertyeditoreditor.cpp  propertyeditorinput.cpp  propertyeditoritem.cpp  propertyeditorlist.cpp

METASOURCES = AUTO
libkformeditor_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KPARTS)

servicetypesdir = $(kde_servicetypesdir)
servicetypes_DATA=widgetfactory.desktop

SUBDIRS = . factories test
