INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)
LDFLAGS = $(all_libraries) $(KDE_RPATH)
METASOURCES = AUTO

kde_module_LTLIBRARIES = clipartthumbnail.la

clipartthumbnail_la_SOURCES = clipartcreator.cpp
clipartthumbnail_la_LIBADD = $(LIB_KOFFICECORE) # for KoClipartCollection
clipartthumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

kde_services_DATA = clipartthumbnail.desktop
