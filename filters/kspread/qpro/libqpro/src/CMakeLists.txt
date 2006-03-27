kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES= -I$(srcdir) -I$(srcdir)/.. $(all_includes)
#
#noinst_LTLIBRARIES = libqpro.la
#libqpro_la_SOURCES = formula.cc  record.cc  record_factory.cc  stream.cc  tablenames.cc
#
