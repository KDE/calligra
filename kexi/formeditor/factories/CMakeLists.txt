

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs ${CMAKE_SOURCE_DIR}/kexi/koproperty ${CMAKE_SOURCE_DIR}/libs/main ${KDE4_INCLUDES}  )


########### next target ###############

set(kformdesigner_containers_PART_SRCS containerfactory.cpp )


kde4_add_plugin(kformdesigner_containers ${kformdesigner_containers_PART_SRCS})



target_link_libraries(
    kformdesigner_containers
    kformdesigner
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTCORE_LIBRARY}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kformdesigner_containers  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kformdesigner_stdwidgets_PART_SRCS stdwidgetfactory.cpp )

kde4_add_plugin(kformdesigner_stdwidgets ${kformdesigner_stdwidgets_PART_SRCS})



target_link_libraries(
    kformdesigner_stdwidgets
    kformdesigner
    koproperty
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kformdesigner_stdwidgets  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kformdesigner_containers.desktop kformdesigner_stdwidgets.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kformdesigner)





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#INCLUDES = -I$(top_srcdir)/kexi -I$(top_srcdir)/kexi/formeditor \
#	-I$(top_srcdir)/kexi/core \
#	-I$(top_srcdir)/lib -I$(top_srcdir)/lib/koproperty -I$(top_srcdir)/lib/komain $(all_includes)
#kde_module_LTLIBRARIES = kformdesigner_containers.la kformdesigner_stdwidgets.la
#kformdesigner_containers_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
#kformdesigner_containers_la_SOURCES =  containerfactory.cpp
#kformdesigner_containers_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la
#
#kformdesigner_stdwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
#kformdesigner_stdwidgets_la_SOURCES =  stdwidgetfactory.cpp
#kformdesigner_stdwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la
#
#
#servicesdir=$(kde_servicesdir)/kformdesigner
#services_DATA = kformdesigner_containers.desktop kformdesigner_stdwidgets.desktop
#
#METASOURCES = AUTO
#
