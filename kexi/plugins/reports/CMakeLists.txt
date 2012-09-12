project(reports)

include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi/widget
    ${CMAKE_SOURCE_DIR}/libs/koreport/
    ${CMAKE_SOURCE_DIR}/libs/koreport/common
    ${CMAKE_SOURCE_DIR}/libs/koreport/renderer
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
    kexireportdesignview.cpp
    kexidbreportdata.cpp
    kexisourceselector.cpp
    krscriptfunctions.cpp
    InternalSourceSelector.cpp
)

if (KEXI_MOBILE)

else (KEXI_MOBILE)
  LIST(APPEND kexihandler_report_SRCS
        keximigratereportdata.cpp
  ) 
endif (KEXI_MOBILE)


qt4_wrap_cpp(kexihandler_report_SRCS ../scripting/kexiscripting/kexiscriptadaptor.h)

kde4_add_plugin(kexihandler_report ${kexihandler_report_SRCS})

target_link_libraries(kexihandler_report
    koreport
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    ${KDE4_KPARTS_LIBS}
    kexidb
    ${CALLIGRADB_LIBS}
    kexicore
    ${KOPROPERTY_LIBS}
    kexiguiutils
    ${KDE4_KROSSCORE_LIBS}
)

if (KEXI_MOBILE)
  
else (KEXI_MOBILE)
    target_link_libraries (kexihandler_report
    keximain
    keximigrate
    )
endif (KEXI_MOBILE)

install(TARGETS kexihandler_report DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kexireporthandler.desktop DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
