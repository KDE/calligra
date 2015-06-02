project(reports)

include_directories(
    ${KOMAIN_INCLUDES}
    ${CMAKE_SOURCE_DIR}/kexi
    ${CMAKE_SOURCE_DIR}/kexi/core
    ${CMAKE_SOURCE_DIR}/kexi/widget
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


qt5_wrap_cpp(kexihandler_report_SRCS ../scripting/kexiscripting/kexiscriptadaptor.h)

add_library(kexihandler_report MODULE ${kexihandler_report_SRCS})

target_link_libraries(kexihandler_report
    Qt5::Gui
    Qt5::Script
    kexicore

    KDb
    KProperty
    KReport

    kexiguiutils
    KF5::KrossCore
    kexiextendedwidgets
)

if (KEXI_MOBILE)
  
else ()
    target_link_libraries (kexihandler_report
    keximain
    keximigrate
    )
endif ()

install(TARGETS kexihandler_report DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
install(FILES kexireporthandler.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra)
