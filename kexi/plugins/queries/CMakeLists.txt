include_directories(${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/widget/tableview
    ${CMAKE_SOURCE_DIR}/lib ${CMAKE_SOURCE_DIR}/libs/main)

# the main plugin
set(kexi_queryplugin_SRCS
   kexiquerypart.cpp
   kexiquerydesignersql.cpp
   kexiquerydesignerguieditor.cpp
   kexiqueryview.cpp
)

add_library(kexi_queryplugin MODULE ${kexi_queryplugin_SRCS})
kcoreaddons_desktop_to_json(kexi_queryplugin kexi_queryplugin.desktop)

target_link_libraries(kexi_queryplugin
    PRIVATE
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
