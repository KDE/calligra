include_directories(${KOMVC_INCLUDES})

set(komvc_LIB_SRCS
    KoApplicationBase.cpp
    KoControllerBase.cpp
    KodocumentBase.cpp
    KoDocumentManager.cpp
    KoMainwindowBase.cpp
    KoViewBase.cpp
    KoDocumentFactory.cpp
    KoViewManagerFactory.cpp
    KoViewManagerBase.cpp
    KoViewFactory.cpp
)

kde4_add_library(komvc SHARED ${komvc_LIB_SRCS})

target_link_libraries(komvc koplugin ${QT_QTGUI_LIBRARY})

set_target_properties(komvc PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)

install(TARGETS komvc  ${INSTALL_TARGETS_DEFAULT_ARGS})

install( FILES
    KoApplicationBase.h
    KoControllerBase.h
    KodocumentBase.h
    KoDocumentManager.h
    KoMainwindowBase.h
    komvc_export.h
    KoViewBase.h
    KoDocumentFactory.h
    KoViewManagerFactory.h
    KoViewManagerBase.h
    KoViewFactory.h
    KoDocumentFactory.h
    DESTINATION
    ${INCLUDE_INSTALL_DIR}
COMPONENT Devel)
