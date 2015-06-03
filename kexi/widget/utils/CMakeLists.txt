include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget/utils   )

########### next target ###############

set(kexiguiutils_LIB_SRCS 
   kexisharedactionclient.cpp 
   kexidisplayutils.cpp 
   kexitooltip.cpp
   kexicontextmenuutils.cpp
   kexidropdownbutton.cpp
   kexicomboboxdropdownbutton.cpp
   kexidatetimeformatter.cpp
   KexiDockableWidget.cpp
)

if (KEXI_MOBILE)

else ()
    list(APPEND kexiguiutils_LIB_SRCS
        kexirecordnavigator.cpp 
    )
endif ()

add_library(kexiguiutils SHARED ${kexiguiutils_LIB_SRCS})

generate_export_header(kexiguiutils)

target_link_libraries(kexiguiutils
kexicore
kexiutils KF5::KIOWidgets
)

set_target_properties(kexiguiutils PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kexiguiutils  ${INSTALL_TARGETS_DEFAULT_ARGS})
