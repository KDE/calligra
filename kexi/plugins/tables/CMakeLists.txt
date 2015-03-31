
include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget
 ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/libs)

########### next target ###############

set(kexihandler_table_PART_SRCS kexitablepart.cpp kexitabledesignerview.cpp
  kexitabledesigner_dataview.cpp kexitabledesignercommands.cpp
  kexitabledesignerview_p.cpp kexilookupcolumnpage.cpp )


add_library(kexihandler_table MODULE ${kexihandler_table_PART_SRCS})



target_link_libraries(
    kexihandler_table
    kexicore
    kexidataviewcommon
    kexidatatable
    kexiextendedwidgets
    kexiutils

    Predicate
    KProperty

    Qt5::Core
    Qt5::Gui
)

install(TARGETS kexihandler_table  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kexitablehandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
