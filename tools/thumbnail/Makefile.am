INCLUDES = $(KOFFICE_INCLUDES) $(all_includes) -I../../kword/ -I../../kpresenter -I../../lib/kotext
LDFLAGS = $(all_libraries) $(KDE_RPATH)
METASOURCES = AUTO

kde_module_LTLIBRARIES = clipartthumbnail.la kofficethumbnail.la

clipartthumbnail_la_SOURCES = clipartcreator.cpp
clipartthumbnail_la_LIBADD = $(LIB_KOFFICECORE) # for KoClipartCollection
clipartthumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

kofficethumbnail_la_SOURCES = kofficecreator.cpp
kofficethumbnail_la_LIBADD = $(LIB_KOFFICECORE)
kofficethumbnail_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

kde_services_DATA = clipartthumbnail.desktop kofficethumbnail.desktop otherofficethumbnail.desktop

