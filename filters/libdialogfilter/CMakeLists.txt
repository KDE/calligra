kde4_header()

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### next target ###############


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#INCLUDES = $(KOFFICE_INCLUDES) $(all_includes)
#
#METASOURCES = AUTO
#
#noinst_LTLIBRARIES = libdialogfilter.la
#libdialogfilter_la_SOURCES = exportsizedia.cpp 
#
