SUBDIRS = toolbar pics

INCLUDES = -I$(srcdir)/../lib $(KOFFICE_INCLUDES) $(all_includes)

bin_PROGRAMS = kudesigner
kudesigner_SOURCES = canvbox.cpp \
	creportitem.cpp \
	itemopts.cpp \
	main.cpp \
	ccalcfield.cpp \
	cspecialfield.cpp \
	kudesigner.cpp \
	mycanvas.cpp \
	cfield.cpp \
	cv.cpp \
	clabel.cpp \
	dlgnew.cpp \
	kudesignerdoc.cpp \
	cline.cpp \
	dlgoptions.cpp \
	kudesignerview.cpp

kudesigner_UIFILES = dlgnew.ui \
	dlgoptions.ui

kudesigner_LDFLAGS = $(all_libraries) $(KDE_RPATH)
kudesigner_LDADD = $(LIB_KIO) $(LIB_KDEPRINT)

appsdir = $(kde_appsdir)/Office
apps_DATA = kudesigner.desktop

kudesigner_METASOURCES = AUTO

rcdir = $(kde_datadir)/kudesigner
rc_DATA = kudesignerui.rc

EXTRA_DIST = \
	report.h \
	canvbox.h \
	clabel.h \
	cv.h \
	itemopts.h \
	mycanvas.h \
	canvdefs.h \
	cline.h \
	dlgnew.h \
	kudesigner.h \
	ccalcfield.h \
	creportitem.h \
	dlgnew.ui.h \
	kudesignerdoc.h \
	cfield.h \
	cspecialfield.h \
	dlgoptions.h \
	kudesignerview.h \
	$(rc_DATA) \
	$(apps_DATA)

messages: 
	$(XGETTEXT) *.cpp -o $(podir)/kudesigner.pot
