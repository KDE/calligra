include_directories(
    ${CMAKE_SOURCE_DIR}/lib
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/widget
    ${KDE4_INCLUDES}
    ${KOKROSS_INCLUDES}
)

########### next target ###############

set(kexihandler_script_PART_SRCS kexiscriptpart.cpp kexiscripteditor.cpp kexiscriptdesignview.cpp )
qt4_wrap_cpp(kexihandler_script_PART_SRCS kexiscriptadaptor.h)
kde4_add_plugin(kexihandler_script ${kexihandler_script_PART_SRCS})
target_link_libraries(
    kexihandler_script
    kexiextendedwidgets
    koproperty
    kokross
    kexicore
    kexiutils
    kexidb
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KIO_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )
install(TARGETS kexihandler_script  DESTINATION ${PLUGIN_INSTALL_DIR})

########### install files ###############

install( FILES  kexiscriptpartui.rc kexiscriptpartinstui.rc  DESTINATION ${DATA_INSTALL_DIR}/kexi)
install( FILES  kexiscripthandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexihandler_script.la
#
#kexihandler_script_la_SOURCES = \
#    kexiscriptpart.cpp kexiscripteditor.cpp kexiscriptdesignview.cpp
#
#kexihandler_script_la_LDFLAGS = \
#    $(KDE_PLUGIN) -module -no-undefined -Wnounresolved $(all_libraries) $(VER_INFO)
#
#kexihandler_script_la_LIBADD = \
#    $(top_builddir)/lib/kross/main/libkrossmain.la \
#    $(top_builddir)/kexi/core/libkexicore.la \
#    $(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#    $(top_builddir)/lib/koproperty/libkoproperty.la
#
#INCLUDES = \
#    $(KOFFICE_INCLUDES) \
#    -I$(top_srcdir)/lib  \
#    -I$(top_srcdir)/kexi/core \
#    -I$(top_srcdir)/kexi \
#    -I$(top_srcdir)/kexi/widget \
#    $(all_includes)
#
#servicesdir=$(kde_servicesdir)/kexi
#services_DATA=kexiscripthandler.desktop
#
#rcdir = $(kde_datadir)/kexi
#rc_DATA = kexiscriptpartui.rc kexiscriptpartinstui.rc
#
#METASOURCES = AUTO
#
#SUBDIRS = .
#
#include ../../Makefile.common
#
#noinst_HEADERS = kexiscriptpart.h kexiscripteditor.h kexiscriptdesignview.h
