INCLUDES = $(KOPROPERTY_INCLUDES) $(KOFFICE_INCLUDES) $(all_includes)

# Temporary hack, to work around the KColorCombo bug with --enable-final
KDE_OPTIONS = nofinal

noinst_LTLIBRARIES =  libkudesignercommon.la
libkudesignercommon_la_LDFLAGS = $(all_libraries)
libkudesignercommon_la_METASOURCES = AUTO
libkudesignercommon_la_SOURCES = band.cpp box.cpp calcfield.cpp canvas.cpp \
	command.cpp commdefs.cpp detailbase.cpp detail.cpp detailfooter.cpp \
	detailheader.cpp field.cpp kugartemplate.cpp label.cpp line.cpp pagefooter.cpp \
	pageheader.cpp propertyserializer.cpp reportfooter.cpp reportheader.cpp reportitem.cpp \
	section.cpp specialfield.cpp structurewidget.cpp view.cpp plugin.cpp
libkudesignercommon_la_LIBADD = $(LIB_KOPROPERTY)

KDE_CXXFLAGS = $(USE_EXCEPTIONS) -DKPROPERTY_EXPORT=

