add_subdirectory( factories )

include_directories(${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/utils
${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/core)

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44010)

# Disable list widget because we need to replace it with QTreeWidget
# which uses very different API compared to Q3ListView.
# todo: re-add QTreeWidget?
add_definitions(-DKEXI_FORMS_NO_LIST_WIDGET)

# Disable pixmap collections
add_definitions(-DKEXI_NO_PIXMAPCOLLECTION)

# enable to add signal/slot connections
# set(KFD_SIGSLOTS true)

########### next target ###############

set(kformdesigner_LIB_SRCS
   container.cpp
   resizehandle.cpp
   widgetfactory.cpp
   widgetlibrary.cpp
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
#KEXI_FORMS_NO_LIST_WIDGET:   editlistviewdialog.cpp
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

  KDb
  KProperty
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
