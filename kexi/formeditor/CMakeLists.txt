add_subdirectory( factories )

include_directories( ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/utils ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDES}  )

add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44010)

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
