
include_directories(${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/libs/main)


########### next target ###############

set(kexihandler_query_PART_SRCS
   kexiquerypart.cpp
   kexiquerydesignersql.cpp
   kexiquerydesignerguieditor.cpp
   kexiqueryview.cpp )

add_library(kexihandler_query MODULE ${kexihandler_query_PART_SRCS})



target_link_libraries(
    kexihandler_query
    kexicore
    kexiextendedwidgets
    kexidataviewcommon
    kexidatatable
    kexirelationsview
    kexiutils

    KDb
    KProperty

    Qt5::Gui
    Qt5::Xml
)

install(TARGETS kexi_queryplugin  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
