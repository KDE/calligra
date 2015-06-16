include_directories(${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget
 ${CMAKE_SOURCE_DIR}/kexi/widget/tableview ${CMAKE_SOURCE_DIR}/libs)

# the main plugin
set(kexi_tableplugin_SRCS
    kexitablepart.cpp
    kexitabledesignerview.cpp
    kexitabledesigner_dataview.cpp
    kexitabledesignercommands.cpp
    kexitabledesignerview_p.cpp
    kexilookupcolumnpage.cpp
)

add_library(kexi_tableplugin MODULE ${kexi_tableplugin_SRCS})
kcoreaddons_desktop_to_json(kexi_tableplugin kexi_tableplugin.desktop)

target_link_libraries(kexi_tableplugin
    PRIVATE
        kexicore
        kexidataviewcommon
        kexidatatable
        kexiextendedwidgets
        kexiutils

        KDb
        KProperty

        Qt5::Core
        Qt5::Gui
)

install(TARGETS kexi_tableplugin DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
