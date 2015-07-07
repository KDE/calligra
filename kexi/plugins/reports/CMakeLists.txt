include_directories(
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi/widget
)

# the main plugin
set(kexi_reportplugin_SRCS
    kexireports.cpp
    kexireportpart.cpp
    kexireportview.cpp
    kexireportdesignview.cpp
    kexidbreportdata.cpp
    kexisourceselector.cpp
    krscriptfunctions.cpp
)

if (KEXI_MOBILE)

else ()
#TODO KEXI3
#  list(APPEND kexi_reportplugin_SRCS
#        keximigratereportdata.cpp
#  )
endif ()

#TODO KEXI3 qt5_wrap_cpp(kexi_reportplugin_SRCS ../scripting/kexiscripting/kexiscriptadaptor.h)

add_library(kexi_reportplugin MODULE ${kexi_reportplugin_SRCS})
kcoreaddons_desktop_to_json(kexi_reportplugin kexi_reportplugin.desktop)

target_link_libraries(kexi_reportplugin
    PRIVATE
        kexicore
        kexiguiutils
        kexiextendedwidgets

        KDb
        KReport
        KProperty

        KF5::KrossCore
        KF5::KDELibs4Support

        Qt5::Gui
        Qt5::Script
)

if (KEXI_MOBILE)

else ()
    target_link_libraries(kexi_reportplugin
      PRIVATE
        keximain
#TODO KEXI3       keximigrate
    )
endif ()

install(TARGETS kexi_reportplugin DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
