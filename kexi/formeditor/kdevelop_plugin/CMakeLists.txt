

include_directories( ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/core ${KDE4_INCLUDES}  )


########### next target ###############

set(kformdesigner_kdev_part_PART_SRCS kfd_kdev_part.cpp )

kde4_automoc(${kformdesigner_kdev_part_PART_SRCS})

kde4_add_plugin(kformdesigner_kdev_part WITH_PREFIX ${kformdesigner_kdev_part_PART_SRCS})



target_link_libraries(kformdesigner_kdev_part  ${KDE4_KDECORE_LIBS} kformdesigner kinterfacedesigner )

install(TARGETS kformdesigner_kdev_part  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformdesigner_kdev_part.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kformdesigner_part.rc kformdesigner_part_shell.rc  DESTINATION ${DATA_INSTALL_DIR}/kformdesigner_kdev_part)





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#INCLUDES = -I$(top_srcdir)/kexi/formeditor -I$(top_srcdir)/kexi/core $(all_includes)
#METASOURCES = AUTO
#
## KFormDesigner KDevelop plugin
#kde_module_LTLIBRARIES = libkformdesigner_kdev_part.la
#
#libkformdesigner_kdev_part_la_SOURCES = kfd_kdev_part.cpp
#libkformdesigner_kdev_part_la_LDFLAGS = -module $(KDE_PLUGIN) $(VER_INFO) $(all_libraries)
#libkformdesigner_kdev_part_la_LIBADD  = $(top_builddir)/kexi/formeditor/libkformdesigner.la \
#	-lkinterfacedesigner $(LIB_KFILE)
#
## this is where the desktop file will go
#partdesktopdir   = $(kde_servicesdir)
#partdesktop_DATA = kformdesigner_kdev_part.desktop
#
## this is where the part's XML-GUI resource file goes
#partrcdir   = $(kde_datadir)/kformdesigner_kdev_part
#partrc_DATA = kformdesigner_part.rc kformdesigner_part_shell.rc
#
