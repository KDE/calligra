
include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget/utils ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/lib/koproperty ${CMAKE_SOURCE_DIR}/lib/komain ${CMAKE_SOURCE_DIR}/kexi/widget/tableview/private ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${KDE4_INCLUDES}  )


########### next target ###############

set(kexihandler_form_PART_SRCS kexiforms.cpp )

kde4_add_plugin(kexihandler_form ${kexihandler_form_PART_SRCS})

target_link_libraries(
    kexihandler_form
    kexicore
    kexiguiutils
    kexidatatable
    kexiextendedwidgets
    kformdesigner
    koproperty
    kexiformutils
    ${KDE4_KDECORE_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kexihandler_form  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kformdesigner_kexidbwidgets_PART_SRCS kexidbfactory.cpp )

kde4_add_plugin(kformdesigner_kexidbwidgets ${kformdesigner_kexidbwidgets_PART_SRCS})



target_link_libraries(
    kformdesigner_kexidbwidgets
    kformdesigner
    kexiformutils
    kexidb
    kexicore
    kexiguiutils
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

install(TARGETS kformdesigner_kexidbwidgets  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kexiformutils_LIB_SRCS 
   kexiformdataiteminterface.cpp 
   kexidataawarewidgetinfo.cpp 
   kexidataprovider.cpp 
   kexiformscrollview.cpp 
   kexiformeventhandler.cpp 
   kexidbtextwidgetinterface.cpp 
   kexiactionselectiondialog.cpp 
   kexiformmanager.cpp 
   kexidatasourcepage.cpp 
   kexiformpart.cpp 
   kexiformview.cpp

  widgets/kexidbutils.cpp
  widgets/kexidbautofield.cpp
  widgets/kexidbform.cpp
  widgets/kexidbsubform.cpp
  widgets/kexidblabel.cpp
  widgets/kexidbimagebox.cpp
  widgets/kexipushbutton.cpp
  widgets/kexiframe.cpp
  widgets/kexidblineedit.cpp
  widgets/kexidbcheckbox.cpp
  widgets/kexidbtextedit.cpp
  widgets/kexidbcombobox.cpp
)

#obsolete	widgets/kexidbdoublespinbox.cpp
#obsolete	widgets/kexidbintspinbox.cpp
#obsolete	widgets/kexidbdatetimeedit.cpp
#obsolete	widgets/kexidbtimeedit.cpp
#obsolete	widgets/kexidbdateedit.cpp


kde4_add_library(kexiformutils SHARED ${kexiformutils_LIB_SRCS})

target_link_libraries(
    kexiformutils
    kexicore
    kexiextendedwidgets
    kformdesigner
    kexidb
    kexiutils
    kexidatatable
    kexiguiutils
    koproperty
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${KDE4_KDE3SUPPORT_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTXML_LIBRARY}
    ${QT_QT3SUPPORT_LIBRARY}
    )

set_target_properties(kexiformutils PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kexiformutils  DESTINATION ${LIB_INSTALL_DIR})


########### install files ###############

install( FILES  kexiformpartui.rc kexiformpartinstui.rc  DESTINATION ${DATA_INSTALL_DIR}/kexi)
install( FILES  kformdesigner_kexidbfactory.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kformdesigner)
install( FILES  kexiformhandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#kde_module_LTLIBRARIES = kexihandler_form.la kformdesigner_kexidbwidgets.la
#
#kexihandler_form_la_SOURCES = kexiforms.cpp
#
#kexihandler_form_la_LDFLAGS = $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module -no-undefined
#kexihandler_form_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
#	$(top_builddir)/kexi/widget/utils/libkexiguiutils.la \
#	$(top_builddir)/kexi/widget/tableview/libkexidatatable.la \
#	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#	$(top_builddir)/kexi/formeditor/libkformdesigner.la \
#	$(top_builddir)/lib/koproperty/libkoproperty.la \
#	./libkexiformutils.la
#
#kformdesigner_kexidbwidgets_la_LDFLAGS =  $(all_libraries) $(KDE_PLUGIN) $(VER_INFO) -module
#kformdesigner_kexidbwidgets_la_SOURCES = kexidbfactory.cpp
#kformdesigner_kexidbwidgets_la_LIBADD = $(top_builddir)/kexi/formeditor/libkformdesigner.la \
#	./libkexiformutils.la
#
#lib_LTLIBRARIES = libkexiformutils.la
#libkexiformutils_la_SOURCES = kexiformdataiteminterface.cpp kexidataawarewidgetinfo.cpp \
#	kexidataprovider.cpp kexiformscrollview.cpp kexiformeventhandler.cpp \
#	kexidbtextwidgetinterface.cpp kexiactionselectiondialog.cpp kexiformmanager.cpp \
#	kexidatasourcepage.cpp kexiformpart.cpp kexiformview.cpp
#libkexiformutils_la_LDFLAGS = $(all_libraries) $(VER_INFO) -Wnounresolved
#libkexiformutils_la_LIBADD = $(top_builddir)/kexi/core/libkexicore.la \
#	$(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#	$(top_builddir)/kexi/plugins/forms/widgets/libkexiformutilswidgets.la
#
#kformdesignerservicesdir=$(kde_servicesdir)/kformdesigner
#kformdesignerservices_DATA=kformdesigner_kexidbfactory.desktop
#
#servicesdir=$(kde_servicesdir)/kexi
#services_DATA=kexiformhandler.desktop
#
#rcdir = $(kde_datadir)/kexi
#rc_DATA = kexiformpartui.rc kexiformpartinstui.rc
#
#SUBDIRS = widgets .
#
#INCLUDES= -I$(top_srcdir)/kexi/core -I$(top_srcdir)/kexi \
#		-I$(top_srcdir)/kexi/widget/utils \
#		-I$(top_srcdir)/kexi/widget \
#		-I$(top_srcdir)/kexi/formeditor \
#		-I$(top_srcdir)/lib -I$(top_srcdir)/lib/koproperty -I$(top_srcdir)/lib/komain \
#		-I$(top_srcdir)/kexi/widget/tableview/private \
#		-I$(top_srcdir)/kexi/widget/tableview $(all_includes)
#
#METASOURCES = AUTO
#
#include ../Makefile.common
#noinst_HEADERS = kexidataprovider.h kexidbfactory.h \
#	kexiformpart.h kexiformscrollview.h kexiformview.h
