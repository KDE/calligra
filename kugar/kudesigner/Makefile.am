SUBDIRS = toolbar pics templates

INCLUDES = -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)


## The kdeinit loadable module
lib_LTLIBRARIES = kudesigner.la
kudesigner_la_SOURCES = canvbox.cpp \
	creportitem.cpp \
	main.cpp \
	ccalcfield.cpp \
	cspecialfield.cpp \
	mycanvas.cpp \
	cfield.cpp \
	cv.cpp \
	clabel.cpp \
	cline.cpp
kudesigner_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version
kudesigner_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KPARTS) $(LIB_KOFFICECORE) $(LIB_KIO) $(LIB_KDEPRINT)

## The part
kde_module_LTLIBRARIES = libkudesignerpart.la
libkudesignerpart_la_SOURCES = kudesigner_part.cpp \
	kudesigner_view.cpp \
	kudesigner_factory.cpp 
libkudesignerpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(KDE_RPATH)
libkudesignerpart_la_LIBADD = kudesigner.la $(LIB_KOFFICEUI) $(LIB_KPARTS) $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = kudesigner
kudesigner_SOURCES = dummy.cc
kudesigner_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kudesigner_LDADD = kudesigner.la $(LIB_KPARTS) $(LIB_KOFFICECORE)

dummy.cc:
	echo > dummy.cc

appsdir = $(kde_appsdir)/Office
apps_DATA = kudesigner.desktop

kdemime_DATA = x-kudesigner.desktop
kdemimedir = $(kde_mimedir)/application

kudesigner_METASOURCES = AUTO

rcdir = $(kde_datadir)/kudesigner
rc_DATA = kudesignerui.rc kudesigner_readonly.rc

EXTRA_DIST = \
	report.h \
	canvbox.h \
	clabel.h \
	cv.h \
	mycanvas.h \
	canvdefs.h \
	cline.h \
	kudesigner_aboutdata.h \
	ccalcfield.h \
	creportitem.h \
	kudesigner_factory.h \
	kudesigner_part.h \
	cfield.h \
	cspecialfield.h \
	kudesigner_view.h \
	$(rc_DATA) \
	$(apps_DATA)

