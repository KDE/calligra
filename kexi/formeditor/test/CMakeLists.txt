kde4_header()

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/kofficecore ${KDE4_INCLUDES}  )


########### next target ###############

set(kformdesigner_SRCS main.cpp kfd_mainwindow.cpp )

kde4_automoc(${kformdesigner_SRCS})

kde4_add_executable(kformdesigner ${kformdesigner_SRCS})

target_link_libraries(kformdesigner  ${KDE4_KDECORE_LIBS} kformdesigner )

install_targets(/bin kformdesigner )


########### next target ###############

set(kformdesigner_part_PART_SRCS kfd_part.cpp )

kde4_automoc(${kformdesigner_part_PART_SRCS})

kde4_add_plugin(kformdesigner_part WITH_PREFIX ${kformdesigner_part_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kformdesigner_part )

target_link_libraries(kformdesigner_part  ${KDE4_KDECORE_LIBS} kformdesigner )

install_targets(${PLUGIN_INSTALL_DIR} kformdesigner_part )


########### install files ###############

install_files( ${SERVICES_INSTALL_DIR} FILES  kformdesigner_part.desktop )
install_files( ${DATA_INSTALL_DIR}/kformdesigner_part FILES  kformdesigner_part.rc kformdesigner_part_shell.rc )
install_files( ${DATA_INSTALL_DIR}/kformdesigner FILES  kfd_mainwindow.rc )
install_files( ${APPLNK_INSTALL_DIR}/Development FILES  kformdesigner.desktop )

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )

kde4_footer()



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
#	-I$(top_srcdir)/lib -I$(top_srcdir)/lib/kofficecore $(all_includes)
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
