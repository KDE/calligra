####### General stuff

INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)

####### Files
# Obviously you have to change "png" (and maybe "import") to
# reflect the name of your filter. If you have more files
# than just pngexport.cc please add them to the _SOUCRES line.

kde_module_LTLIBRARIES = libpngexport.la

libpngexport_la_SOURCES = pngexport.cpp
libpngexport_la_LDFLAGS	= -module $(KDE_PLUGIN)
libpngexport_la_LIBADD = $(KOFFICE_LIBS)

METASOURCES = AUTO

service_DATA = kchart_png_export.desktop
servicedir = $(kde_servicesdir)

# Note: If your filter exports or exports some special file
# which KDE doesn't have a mimetype for, yet, you'll have to
# create a mimetype and install it using those two lines.
# In case of doubt please ask koffice@mail.kde.org or
# koffice-devel@mail.kde.org. Thanks.
# data_DATA = x-png.desktop
# datadir = $(kde_mimedir)/text
