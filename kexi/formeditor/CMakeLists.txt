add_subdirectory( factories )

include_directories(${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/utils
${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/core)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44010)

# enable to add signal/slot connections
# set(KFD_SIGSLOTS true)

########### next target ###############

set(kformdesigner_LIB_SRCS
   container.cpp
   resizehandle.cpp
   widgetfactory.cpp
   widgetlibrary.cpp
   KexiFormWidgetsPluginMetaData.cpp
   WidgetInfo.cpp
   libactionwidget.cpp
   form.cpp
   form_p.cpp
   objecttree.cpp
   formIO.cpp
   FormWidget.cpp
   FormWidgetInterface.cpp
   WidgetTreeWidget.cpp
   commands.cpp
   events.cpp
   richtextdialog.cpp
   tabstopdialog.cpp
#KEXI_LIST_FORM_WIDGET_SUPPORT:   editlistviewdialog.cpp
   utils.cpp
#todo   kfdpixmapedit.cpp
   widgetwithsubpropertiesinterface.cpp
   kexiformeventhandler.cpp # from libkexiformutils
   kexiactionselectiondialog.cpp # from libkexiformutils
)

set(kformdesigner_LIBS
  kexiutils
  kexicore
  kexiextendedwidgets
  kundo2
  kowidgets # KoFileDialog

  KDb
  KPropertyWidgets
)

if(KFD_SIGSLOTS)
  add_definitions(
    -DKFD_SIGSLOTS=1
  )
  list(APPEND kformdesigner_LIB_SRCS connectiondialog.cpp)
  list(APPEND kformdesigner_LIBS kexiextendedwidgets kexidatatable)
endif()

add_library(kformdesigner SHARED ${kformdesigner_LIB_SRCS})

generate_export_header(kformdesigner)

target_link_libraries(kformdesigner ${kformdesigner_LIBS})

set_target_properties(kformdesigner PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kformdesigner  ${INSTALL_TARGETS_DEFAULT_ARGS})
