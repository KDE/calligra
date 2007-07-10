

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/libs/komain ${KDE4_INCLUDES}  )


########### next target ###############

set(kformdesigner_SRCS main.cpp kfd_mainwindow.cpp )

kde4_automoc(${kformdesigner_SRCS})

kde4_add_executable(kformdesigner ${kformdesigner_SRCS})

target_link_libraries(kformdesigner  ${KDE4_KDECORE_LIBS} kformdesigner )

install(TARGETS kformdesigner  DESTINATION ${BIN_INSTALL_DIR})


########### next target ###############

set(kformdesigner_part_PART_SRCS kfd_part.cpp )

kde4_add_plugin(kformdesigner_part WITH_PREFIX ${kformdesigner_part_PART_SRCS})



target_link_libraries(kformdesigner_part  ${KDE4_KDECORE_LIBS} kformdesigner )

install(TARGETS kformdesigner_part  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformdesigner_part.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kformdesigner_part.rc kformdesigner_part_shell.rc  DESTINATION ${DATA_INSTALL_DIR}/kformdesigner_part)
install( FILES  kfd_mainwindow.rc  DESTINATION ${DATA_INSTALL_DIR}/kformdesigner)
install( FILES  kformdesigner.desktop  DESTINATION ${APPLNK_INSTALL_DIR}/Development)

kde4_install_icons( ${ICON_INSTALL_DIR}   )





#original Makefile.am contents follow:

### Makefile.am for kformdesigner
#
#include $(top_srcdir)/kexi/Makefile.global
#
## this is the program that gets installed.  it's name is used for all
## of the other Makefile.am variables
#bin_PROGRAMS = kformdesigner
#
## set the include path for X, qt and KDE
#INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/formeditor \
#	-I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/core \
#	-I$(top_srcdir)/lib -I$(top_srcdir)/lib/komain $(all_includes)
#
#kformdesigner_LDFLAGS = $(KDE_RPATH) $(all_libraries)
#
#kformdesigner_LDADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la
#
#kformdesigner_SOURCES = main.cpp kfd_mainwindow.cpp
#
## client stuff
#
#METASOURCES = AUTO
#
#kdelnkdir = $(kde_appsdir)/Development
#kdelnk_DATA = kformdesigner.desktop
#
#rcdir = $(kde_datadir)/kformdesigner
#rc_DATA = kfd_mainwindow.rc
#
#
## KFormDesigner KPart
#kde_module_LTLIBRARIES = libkformdesigner_part.la
#
#libkformdesigner_part_la_SOURCES = kfd_part.cpp
#libkformdesigner_part_la_LDFLAGS = -module $(KDE_PLUGIN) $(all_libraries) $(VER_INFO)
#libkformdesigner_part_la_LIBADD  = $(top_builddir)/kexi/formeditor/libkformdesigner.la $(LIB_KFILE)
#
## this is where the desktop file will go
#partdesktopdir   = $(kde_servicesdir)
#partdesktop_DATA = kformdesigner_part.desktop
#
## this is where the part's XML-GUI resource file goes
#partrcdir   = $(kde_datadir)/kformdesigner_part
#partrc_DATA = kformdesigner_part.rc kformdesigner_part_shell.rc
#
#KDE_ICON = kformdesigner
