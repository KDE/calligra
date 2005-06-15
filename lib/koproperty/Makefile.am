INCLUDES = -I$(srcdir)/editors $(all_includes)

lib_LTLIBRARIES =  libkoproperty.la

libkoproperty_la_LIBADD = $(LIB_KDEUI) ./editors/libkopropertyeditors.la
libkoproperty_la_LDFLAGS = -no-undefined $(all_libraries)
libkoproperty_la_SOURCES = property.cpp customproperty.cpp list.cpp editor.cpp \
	editoritem.cpp factory.cpp widget.cpp

METASOURCES = AUTO

SUBDIRS = editors .

KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DKPROPERTY_EXPORT=
