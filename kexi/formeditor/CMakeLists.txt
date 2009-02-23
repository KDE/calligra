add_subdirectory( factories )

include_directories( ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/utils ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDES}  )

########### next target ###############

set(kformdesigner_LIB_SRCS 
   container.cpp 
   resizehandle.cpp 
   widgetfactory.cpp 
   widgetlibrary.cpp 
   libactionwidget.cpp 
   form.cpp 
   objecttree.cpp 
   formIO.cpp 
   FormWidget.cpp
   objecttreeview.cpp 
   spring.cpp 
   commands.cpp 
   events.cpp 
   richtextdialog.cpp 
   tabstopdialog.cpp 
   editlistviewdialog.cpp 
   utils.cpp 
#todo   kfdpixmapedit.cpp
   widgetwithsubpropertiesinterface.cpp
#removed in 2.0 formmanager.cpp
#removed in 2.0 widgetpropertyset.cpp 
   kexiformeventhandler.cpp # from libkexiformutils
   kexiactionselectiondialog.cpp # from libkexiformutils
)

set(kformdesigner_LIBS
  ${KOPROPERTY_LIBS}
  kexiutils
  kexicore
  kexiextendedwidgets
  ${KDE4_KDE3SUPPORT_LIBS}
  ${KDE4_KPARTS_LIBS}
)

# enable to add signal/slot connections
# set(KFD_SIGSLOTS true)

if(KFD_SIGSLOTS)
  ADD_DEFINITIONS( 
    -DKFD_SIGSLOTS=1
  )
  list(APPEND kformdesigner_LIB_SRCS connectiondialog.cpp)
  list(APPEND kformdesigner_LIBS kexiextendedwidgets kexidatatable)
endif(KFD_SIGSLOTS)

kde4_add_library(kformdesigner SHARED ${kformdesigner_LIB_SRCS})

target_link_libraries(kformdesigner ${kformdesigner_LIBS})

set_target_properties(kformdesigner PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kformdesigner  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES  widgetfactory.desktop  DESTINATION ${SERVICETYPES_INSTALL_DIR})





#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#
#INCLUDES = -I$(top_srcdir)/kexi/widget -I$(top_srcdir)/kexi/widget/utils \
#  -I$(top_srcdir)/kexi/widget/tableview \
#  -I$(top_srcdir)/kexi/core \
#  -I$(top_srcdir)/kexi \
#  -I$(top_srcdir)/lib/komain \
#  -I$(top_srcdir)/lib -I$(top_srcdir)/lib/koproperty $(all_includes)
#
#lib_LTLIBRARIES =  libkformdesigner.la
#libkformdesigner_la_LDFLAGS = -no-undefined $(all_libraries) $(VER_INFO)
#libkformdesigner_la_SOURCES = container.cpp resizehandle.cpp widgetfactory.cpp \
#   widgetlibrary.cpp libactionwidget.cpp form.cpp \
#  objecttree.cpp formIO.cpp formmanager.cpp objecttreeview.cpp spring.cpp \
#  commands.cpp events.cpp connectiondialog.cpp richtextdialog.cpp \
#  tabstopdialog.cpp editlistviewdialog.cpp utils.cpp widgetpropertyset.cpp \
#  kfdpixmapedit.cpp
#
#METASOURCES = AUTO
#libkformdesigner_la_LIBADD = \
#  $(top_builddir)/kexi/widget/libkexiextendedwidgets.la \
#  $(top_builddir)/kexi/widget/utils/libkexiguiutils.la \
#  $(top_builddir)/kexi/widget/tableview/libkexidatatable.la \
#  $(top_builddir)/lib/koproperty/libkoproperty.la \
#  $(LIB_KPARTS)
#
#servicetypesdir = $(kde_servicetypesdir)
#servicetypes_DATA=widgetfactory.desktop
#
#SUBDIRS = . factories #test #scripting #kdevelop_plugin
#
#messages: rc.cpp
#	$(EXTRACTRC) `find . -name "*.rc" -o -name "*.ui"` >> rc.cpp
#	$(XGETTEXT) `find . -name "*.cpp" -o -name "*.h"` -o $(podir)/kformdesigner.pot
