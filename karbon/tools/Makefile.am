INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
	-I$(srcdir)/.. \
	-I$(srcdir)/../commands \
	-I$(srcdir)/../core \
	-I$(srcdir)/../dialogs \
	-I$(srcdir)/../widgets \
	-I$(srcdir)/../dockers \
	-I$(srcdir)/../render \
	-I$(srcdir)/../shapes \
	-I$(srcdir)/../visitors \
	$(all_includes)

noinst_LTLIBRARIES = libkarbontools.la
noinst_HEADERS = vtool.h vshapetool.h
libkarbontools_la_SOURCES = vtool.cc vshapetool.cc

kde_module_LTLIBRARIES = karbon_selecttoolplugin.la karbon_rotatetoolplugin.la karbon_selectnodestoolplugin.la karbon_sheartoolplugin.la karbon_rectangletoolplugin.la karbon_startoolplugin.la karbon_roundrecttoolplugin.la karbon_sinustoolplugin.la  karbon_spiraltoolplugin.la karbon_ellipsetoolplugin.la karbon_polygontoolplugin.la karbon_polylinetoolplugin.la karbon_gradienttoolplugin.la karbon_texttoolplugin.la karbon_patterntoolplugin.la karbon_cliparttoolplugin.la

karbon_selecttoolplugin_la_SOURCES = vselecttool.cc
karbon_selecttoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_selecttoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_selectnodestoolplugin_la_SOURCES = vselectnodestool.cc
karbon_selectnodestoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_selectnodestoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_rotatetoolplugin_la_SOURCES = vrotatetool.cc
karbon_rotatetoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_rotatetoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_sheartoolplugin_la_SOURCES = vsheartool.cc
karbon_sheartoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_sheartoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_rectangletoolplugin_la_SOURCES = vrectangletool.cc
karbon_rectangletoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_rectangletoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_roundrecttoolplugin_la_SOURCES = vroundrecttool.cc
karbon_roundrecttoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_roundrecttoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_startoolplugin_la_SOURCES = vstartool.cc
karbon_startoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_startoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_sinustoolplugin_la_SOURCES = vsinustool.cc
karbon_sinustoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_sinustoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_spiraltoolplugin_la_SOURCES = vspiraltool.cc
karbon_spiraltoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_spiraltoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_ellipsetoolplugin_la_SOURCES = vellipsetool.cc
karbon_ellipsetoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_ellipsetoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_polygontoolplugin_la_SOURCES = vpolygontool.cc
karbon_polygontoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_polygontoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_polylinetoolplugin_la_SOURCES = vpolylinetool.cc
karbon_polylinetoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_polylinetoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_gradienttoolplugin_la_SOURCES = vgradienttool.cc
karbon_gradienttoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_gradienttoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_texttoolplugin_la_SOURCES = vtexttool.cc
karbon_texttoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_texttoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_patterntoolplugin_la_SOURCES = vpatterntool.cc
karbon_patterntoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_patterntoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

karbon_cliparttoolplugin_la_SOURCES = vcliparttool.cc
karbon_cliparttoolplugin_la_LIBADD = libkarbontools.la ../libkarbonpart.la $(LIB_KPARTS)
karbon_cliparttoolplugin_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)

datadir   = $(kde_datadir)/karbon/kpartplugins
data_DATA = selecttoolplugin.rc rotatetoolplugin.rc selectnodestoolplugin.rc sheartoolplugin.rc rectangletoolplugin.rc roundrecttoolplugin.rc startoolplugin.rc sinustoolplugin.rc spiraltoolplugin.rc ellipsetoolplugin.rc polygontoolplugin.rc polylinetoolplugin.rc gradienttoolplugin.rc texttoolplugin.rc patterntoolplugin.rc cliparttoolplugin.rc

METASOURCES = AUTO

