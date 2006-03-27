kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#
#INCLUDES= -I$(srcdir)
#
#noinst_LTLIBRARIES = libgoo.la
#
#libgoo_la_SOURCES = GHash.cc GList.cc GString.cc gmempp.cc gfile.cc \
#                    gmem.c parseargs.c
