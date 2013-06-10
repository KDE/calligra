add_subdirectory( tests )

add_definitions(${KDE4_ENABLE_EXCEPTIONS})

include_directories( ${KOMVC_INCLUDES})

set(KOMVC_LIB_SRCS
    KoDocumentBase.cpp
    KoControllerBase.cpp
    KoViewBase.cpp
    KoMainWindowBase.cpp
    KoApplicationBase.cpp
)

#kde4_add_ui_files( komvc_LIB_SRCS)
kde4_add_library(komvc SHARED ${KOMVC_LIB_SRCS})

target_link_libraries(komvc kundo2 ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

target_link_libraries(komvc LINK_INTERFACE_LIBRARIES kundo2 ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

set_target_properties(komvc PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS komvc ${INSTALL_TARGETS_DEFAULT_ARGS} )

install( FILES
    KoDocumentBase.h
    KoViewBase.h
    KoMainWindowBase.h
    KoApplicationBase.h
DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

