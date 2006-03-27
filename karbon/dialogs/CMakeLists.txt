kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(KOPAINTER_INCLUDES) \
#	-I$(srcdir)/.. \
#	-I$(srcdir)/../core \
#	-I$(srcdir)/../commands \
#	$(all_includes)
#
#noinst_LTLIBRARIES = \
#	libkarbondialogs.la
#
#noinst_HEADERS = \
#	vcolordlg.h \
#	vconfiguredlg.h \
#	vcolortab.h
#
#libkarbondialogs_la_SOURCES = \
#	vcolordlg.cc \
#	vconfiguredlg.cc \
#	vcolortab.cc
#
#libkarbondialogs_la_METASOURCES = \
#	AUTO
#
