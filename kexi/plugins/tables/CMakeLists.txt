
include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget
 ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/kexi/kexidb ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDES}  )

########### next target ###############

set(kexihandler_table_PART_SRCS kexitablepart.cpp kexitabledesignerview.cpp
  kexitabledesigner_dataview.cpp kexitabledesignercommands.cpp
  kexitabledesignerview_p.cpp kexilookupcolumnpage.cpp )


add_library(kexihandler_table MODULE ${kexihandler_table_PART_SRCS})



target_link_libraries(
    kexihandler_table
    kexicore
    kexidb
    ${CALLIGRADB_LIBS}
    kexidataviewcommon
    kexidatatable
    kexiextendedwidgets
    ${KOPROPERTY_LIBS}
    kexiutils
    
    ${KDE4_KDEUI_LIBS}
    Qt5::Core
    Qt5::Gui
    )

install(TARGETS kexihandler_table  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexitablehandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
