if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

########### libkostore ###############

set(kostore_LIB_SRCS
    KoDirectoryStore.cpp
    KoEncryptedStore.cpp
    KoLZF.cpp
    KoStore.cpp
    KoStoreDevice.cpp
    KoTarStore.cpp
    KoXmlNS.cpp
    KoXmlReader.cpp
    KoXmlWriter.cpp
    KoZipStore.cpp
    StoreDebug.cpp
    KoNetAccess.cpp # temporary while porting
)

add_library(kostore SHARED ${kostore_LIB_SRCS})
generate_export_header(kostore BASE_NAME kostore)

target_link_libraries(kostore
    PUBLIC
        Qt6::Xml
        Qt6::Core
        Qt6::Widgets
        KF6::KIOCore
    PRIVATE
        Qt6::Gui
        KF6::Archive
        KF6::KIOWidgets
        KF6::WidgetsAddons
        KF6::I18n
        OpenSSL::SSL
        ${QTKEYCHAIN_LIBRARIES}
)

set_target_properties(kostore PROPERTIES
    VERSION ${CALLIGRA_VERSION} SOVERSION ${CALLIGRA_SOVERSION}
)
install(TARGETS kostore ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} )

if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kostore_export.h
    KoStore.h
DESTINATION ${KDE_INSTALL_INCLUDEDIR}/calligra COMPONENT Devel)

endif()
