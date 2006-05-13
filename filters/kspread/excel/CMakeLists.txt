

add_subdirectory( sidewinder )
add_subdirectory( import )

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES}  )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  )





#original Makefile.am contents follow:

######## General stuff
#
#INCLUDES= -I$(srcdir) -I$(top_srcdir)/kspread $(KOFFICE_INCLUDES) \
#	$(KOTEXT_INCLUDES) $(all_includes)
#
#SUBDIRS = sidewinder import
#
######## Files
#
#METASOURCES = AUTO
#
#servicedir = $(kde_servicesdir)
#
#messages: rc.cpp
#	$(XGETTEXT) *.cc *.cpp -o $(podir)/kspreadexcelfilter.pot
