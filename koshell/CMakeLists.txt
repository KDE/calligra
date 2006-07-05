
project(koshell)
include_directories(${KOFFICEUI_INCLUDES})


########### next target ###############

set(koshell_KDEINIT_SRCS koshell_main.cc koshell_shell.cc iconsidepane.cpp )

kde4_automoc(${koshell_KDEINIT_SRCS})

kde4_add_kcfg_files(koshell_KDEINIT_SRCS koshellsettings.kcfgc )

kde4_add_kdeinit_executable( koshell ${koshell_KDEINIT_SRCS})

target_link_libraries(kdeinit_koshell  ${KDE4_KDECORE_LIBS} kofficeui )

install(TARGETS kdeinit_koshell  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( koshell kdeinit_koshell )
install(TARGETS koshell  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  koshell.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES  koshell.kcfg  DESTINATION ${KCFG_INSTALL_DIR})
install( FILES  koshellui.rc  DESTINATION ${DATA_INSTALL_DIR}/koshell)

kde4_install_icons( ${ICON_INSTALL_DIR}  hicolor )







#original Makefile.am contents follow:

#
#INCLUDES  = $(KOFFICE_INCLUDES) $(all_includes)
#
### The kdeinit loadable module and executable
#lib_LTLIBRARIES = 
#kdeinit_LTLIBRARIES = koshell.la
#bin_PROGRAMS =
#koshell_la_SOURCES = koshell_main.cc koshell_shell.cc iconsidepane.cpp koshellsettings.kcfgc
#koshell_la_LDFLAGS = $(all_libraries) -module $(KDE_PLUGIN)
#koshell_la_LIBADD = $(LIB_KOFFICEUI)
#kde_kcfg_DATA = koshell.kcfg
#
#METASOURCES = AUTO
#
#KDE_ICON=AUTO
#
#rcdir = $(kde_datadir)/koshell
#rc_DATA = koshellui.rc
#
#applnk_DATA = koshell.desktop
#applnkdir = $(kde_appsdir)/Office
#
#messages: rc.cpp
#	$(XGETTEXT) rc.cpp *.cc *.cpp -o $(podir)/koshell.pot
#
#include $(top_srcdir)/admin/Doxyfile.am
