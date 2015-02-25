include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget/utils ${KDE4_INCLUDES}  )

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

IF (KEXI_MOBILE)

ELSE (KEXI_MOBILE)
    LIST(APPEND kexiguiutils_LIB_SRCS
        kexirecordnavigator.cpp 
    )
ENDIF (KEXI_MOBILE)

kde4_add_library(kexiguiutils SHARED ${kexiguiutils_LIB_SRCS})

target_link_libraries(kexiguiutils
kexicore
kexiutils ${KDE4_KIO_LIBS}
)

set_target_properties(kexiguiutils PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kexiguiutils  ${INSTALL_TARGETS_DEFAULT_ARGS})
