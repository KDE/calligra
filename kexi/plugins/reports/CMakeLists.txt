project(reports)

include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi/widget
    ${KOREPORT_INCLUDES}
    ${KDE4_INCLUDES}
)

#build a shared library
set(kexihandler_report_SRCS 
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
  list(APPEND kexihandler_report_SRCS
        keximigratereportdata.cpp
  ) 
endif ()


qt4_wrap_cpp(kexihandler_report_SRCS ../scripting/kexiscripting/kexiscriptadaptor.h)

kde4_add_plugin(kexihandler_report ${kexihandler_report_SRCS})

target_link_libraries(kexihandler_report
    ${KOREPORT_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${QT_QTGUI_LIBRARY}
    ${QT_QTSCRIPT_LIBRARY}
    kexidb
    ${CALLIGRADB_LIBS}
    kexicore
    ${KOPROPERTY_LIBS}
    kexiguiutils
    kexiextendedwidgets
    ${KDE4_KROSSCORE_LIBS}
)

if (KEXI_MOBILE)
  
else ()
    target_link_libraries (kexihandler_report
    keximain
    keximigrate
    )
endif ()

install(TARGETS kexihandler_report DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES kexireporthandler.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
