kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES = -I$(srcdir)/.. $(KOFFICE_INCLUDES) $(all_includes)
#
######## Files
#
#noinst_LTLIBRARIES = libaf.la
#
#libaf_la_SOURCES = ATFInterpreter.cpp AFChoose.cpp
#
#METASOURCES = AUTO
#
