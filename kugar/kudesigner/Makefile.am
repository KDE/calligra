SUBDIRS = toolbar pics templates

INCLUDES = -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)


## The kdeinit loadable module
lib_LTLIBRARIES = libkudesignercore.la kudesigner.la
libkudesignercore_la_SOURCES = canvbox.cpp 	creportitem.cpp 	ccalcfield.cpp 	cspecialfield.cpp 	mycanvas.cpp 	cfield.cpp 	cv.cpp 	clabel.cpp 	cline.cpp 	property.cpp 	propertyeditor.cpp 	propertywidgets.cpp 	kudesigner_view.cpp 	kudesigner_factory.cpp 	kudesigner_doc.cpp 	plugin.cpp kudesigner_command.h kudesigner_command.cpp canvsection.h canvsection.cpp canvband.h canvband.cpp canvkutemplate.h canvkutemplate.cpp canvreportheader.h canvreportheader.cpp canvreportfooter.h canvreportfooter.cpp canvpageheader.h canvpageheader.cpp canvpagefooter.h canvpagefooter.cpp canvdetail.h canvdetail.cpp canvdetailheader.h canvdetailheader.cpp canvdetailfooter.h canvdetailfooter.cpp
libkudesignercore_la_LDFLAGS = $(all_libraries) $(KDE_RPATH) -module -avoid-version
libkudesignercore_la_LIBADD = $(LIB_KOFFICEUI) $(LIB_KPARTS) $(LIB_KOFFICECORE) $(LIB_KIO) $(LIB_KDEPRINT)

## The part
kde_module_LTLIBRARIES = libkudesignerpart.la
libkudesignerpart_la_SOURCES = kudesigner_part.cpp canvband.h canvdetailfooter.h canvdetail.h canvdetailheader.h canvkutemplate.h canvpagefooter.h canvpageheader.h canvreportfooter.h canvreportheader.h canvsection.h
libkudesignerpart_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(KDE_RPATH)
libkudesignerpart_la_LIBADD = libkudesignercore.la $(LIB_KOFFICEUI) $(LIB_KPARTS) $(LIB_KOFFICECORE)

## The kdeinit loadable module
kudesigner_la_SOURCES = main.cpp canvband.h canvdetailfooter.h canvdetail.h canvdetailheader.h canvkutemplate.h canvpagefooter.h canvpageheader.h canvreportfooter.h canvreportheader.h canvsection.h
kudesigner_la_LDFLAGS = $(all_libraries) -module -avoid-version
kudesigner_la_LIBADD = $(LIB_KOFFICECORE)

## The executable
bin_PROGRAMS = kudesigner
kudesigner_SOURCES = dummy.cc
kudesigner_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kudesigner_LDADD = kudesigner.la

dummy.cc:
	echo > dummy.cc

appsdir = $(kde_appsdir)/Office
apps_DATA = kudesigner.desktop

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
	property.h \
	propertyeditor.h \
	propertywidgets.h \
	$(rc_DATA) \
	$(apps_DATA)

