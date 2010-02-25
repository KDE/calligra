project(reports)

add_subdirectory( koreport )

include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi/widget
    ${CMAKE_SOURCE_DIR}/kexi/plugins/reports/koreport/
    ${CMAKE_SOURCE_DIR}/kexi/plugins/reports/koreport/common
    ${CMAKE_SOURCE_DIR}/kexi/plugins/reports/koreport/renderer
    ${KDE4_INCLUDES}
)

if (SHOULD_BUILD_KCHART)
    include_directories(${CMAKE_SOURCE_DIR}/kchart/kdchart/include)
endif (SHOULD_BUILD_KCHART)

#build a shared library
set(kexihandler_report_SRCS
    kexireports.cpp
    kexireportpart.cpp
    kexireportview.cpp
    kexireportpage.cpp
    kexireportdesignview.cpp
    kexidbreportdata.cpp
    keximigratereportdata.cpp
    kexisourceselector.cpp
    krscriptfunctions.cpp
)

qt4_wrap_cpp(kexihandler_report_SRCS ../scripting/kexiscripting/kexiscriptadaptor.h)

kde4_add_plugin(kexihandler_report ${kexihandler_report_SRCS})

target_link_libraries(kexihandler_report
    koreport
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${KDE4_KPARTS_LIBS}
    kexidb
    kexicore
    keximain
    ${KOPROPERTY_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
    kexiguiutils
    keximigrate
    ${KDE4_KROSSCORE_LIBS}
)

if (SHOULD_BUILD_KSPREAD)
    target_link_libraries(kexihandler_report kspreadcommon)
endif (SHOULD_BUILD_KSPREAD)

install(TARGETS kexihandler_report DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kexireporthandler.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
