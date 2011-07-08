add_subdirectory( factories )

include_directories( ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/utils ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDES}  )

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44010)

# Disable list widget because we need to replace it with QTreeWidget 
# which uses very different API compared to Q3ListView.
# todo: re-add QTreeWidget?
add_definitions(-DKEXI_FORMS_NO_LIST_WIDGET)

########### next target ###############

set(kformdesigner_LIB_SRCS 
   container.cpp
   resizehandle.cpp
   widgetfactory.cpp
   widgetlibrary.cpp
   WidgetInfo.cpp
   libactionwidget.cpp
   form.cpp
   objecttree.cpp
   formIO.cpp
   FormWidget.cpp
   FormWidgetInterface.cpp
   WidgetTreeWidget.cpp
#todo   spring.cpp
   commands.cpp
   events.cpp
   richtextdialog.cpp
   tabstopdialog.cpp
#KEXI_FORMS_NO_LIST_WIDGET:   editlistviewdialog.cpp
   utils.cpp
#todo   kfdpixmapedit.cpp
   widgetwithsubpropertiesinterface.cpp
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

set_target_properties(kformdesigner PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kformdesigner  ${INSTALL_TARGETS_DEFAULT_ARGS})


########### install files ###############

install( FILES
  FormWidget.h
  FormWidgetInterface.h
  objecttree.h
  WidgetTreeWidget.h
  widgetfactory.h
  WidgetInfo.h
  widgetlibrary.h
  widgetwithsubpropertiesinterface.h
  DESTINATION ${INCLUDE_INSTALL_DIR}/kexi/forms COMPONENT Devel
)

install( FILES
  widgetfactory.desktop
  DESTINATION ${SERVICETYPES_INSTALL_DIR}
)
