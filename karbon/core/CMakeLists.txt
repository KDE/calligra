




########### next target ###############


########### install files ###############






#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../render \
#	-I$(srcdir)/../visitors \
#	$(LIBFREETYPE_CFLAGS) $(LIBFONTCONFIG_CFLAGS) $(all_includes)
#
#noinst_LTLIBRARIES = libkarboncore.la
#
#noinst_HEADERS = \
#	vcolor.h \
#	vcomposite.h \
#	vcomposite_iface.h \
#	vdashpattern.h \
#	vdocument.h \
#	vfill.h \
#	vglobal.h \
#	vgradient.h \
#	vgroup.h \
#	vobject_iface.h \
#	vgroup_iface.h \
#	vimage.h \
#	vkarbonplugin.h \
#	vlayer.h \
#	vobject.h \
#	vpath.h \
#	vpattern.h \
#	vsegment.h \
#	vselection.h \
#	vstroke.h \
#	vvisitor.h \
#	vlayer_iface.h \
#	vtext.h \
#	vtext_iface.h \
#	vclipgroup.h \
#	vcursor.h
#
#libkarboncore_la_SOURCES = \
#	vcolor.cc \
#	vcomposite.cc \
#	vcomposite_iface.cc \
#	vcomposite_iface.skel \
#	vdashpattern.cc \
#	vdocument.cc \
#	vfill.cc \
#	vglobal.cc \
#	vgradient.cc \
#	vgroup.cc \
#	vobject_iface.cc \
#	vobject_iface.skel \
#	vgroup_iface.cc \
#	vgroup_iface.skel \
#	vimage.cc \
#	vkarbonplugin.cc \
#	vlayer.cc \
#	vobject.cc \
#	vpath.cc \
#	vpattern.cc \
#	vsegment.cc \
#	vselection.cc \
#	vstroke.cc \
#	vvisitor.cc \
#	vlayer_iface.cc \
#	vlayer_iface.skel \
#	vtext.cc \
#	vtext_iface.cc \
#	vtext_iface.skel \
#	vclipgroup.cc \
#	vcursor.cc
#
#libkarboncore_la_LIBADD = $(LIBFONTCONFIG_LIBS)
#
#libkarboncore_la_METASOURCES = \
#	AUTO
