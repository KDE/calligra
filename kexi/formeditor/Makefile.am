INCLUDES = $(all_includes)
lib_LTLIBRARIES =  libkformeditor.la
libkformeditor_la_LDFLAGS = -no-undefined $(all_libraries)
libkformeditor_la_SOURCES = container.cpp resizehandle.cpp widgetfactory.cpp widgetlibrary.cpp libactionwidget.cpp form.cpp objecttree.cpp propertybuffer.cpp  propertybufferitem.cpp  propertyeditor.cpp  propertyeditoreditor.cpp  propertyeditorinput.cpp  propertyeditoritem.cpp  propertyeditorlist.cpp eventdetail.ui

METASOURCES = AUTO
libkformeditor_la_LIBADD = $(LIB_QT) $(LIB_KDECORE) $(LIB_KDEUI) $(LIB_KPARTS)

servicetypesdir = $(kde_servicetypesdir)
servicetypes_DATA=widgetfactory.desktop

SUBDIRS = . factories test

KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DKEXI_HAND_TBL_EXPORT= -DKEXICORE_EXPORT= -DKEXIDB_EXPORT= \
-DKEXIFILTER_EXPORT= -DKFORMEDITOR_EXPORT= -DKEXI_HAND_TBL_EXPORT= -DKEXI_HAND_QUERY_EXPORT= \
-DKEXI_HAND_RELAT_EXPORT= -DKEXI_HAND_FORM_EXPORT= -DKEXIEXTWIDGETS_EXPORT= -DKEXIDATATABLE_EXPORT=
