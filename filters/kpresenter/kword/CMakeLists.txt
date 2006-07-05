




########### next target ###############

set(kprkword_PART_SRCS kprkword.cc )

kde4_automoc(${kprkword_PART_SRCS})

kde4_add_plugin(kprkword WITH_PREFIX ${kprkword_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kprkword )

target_link_libraries(kprkword  ${KDE4_KDECORE_LIBS} kofficecore)

install(TARGETS kprkword  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kprkword.desktop  DESTINATION ${SERVICES_INSTALL_DIR})





#original Makefile.am contents follow:

#INCLUDES= -I$(srcdir) $(KOFFICE_INCLUDES) $(all_includes)
#
#kde_module_LTLIBRARIES = libkprkword.la
#
#libkprkword_la_SOURCES = kprkword.cc
#libkprkword_la_LDFLAGS	= $(all_libraries) -module -avoid-version -no-undefined
#libkprkword_la_LIBADD = $(KOFFICE_LIBS) 
#
#METASOURCES = AUTO
#
#service_DATA = kprkword.desktop
#servicedir = $(kde_servicesdir)
