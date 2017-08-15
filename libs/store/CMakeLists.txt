if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

########### libkostore ###############

if( Qca-qt5_FOUND )
    add_definitions( -DQCA2 )
endif()

set(kostore_LIB_SRCS
    KoDirectoryStore.cpp
    KoEncryptedStore.cpp
    KoEncryptionChecker.cpp
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
        Qt5::Xml
        Qt5::Core
        KF5::KIOCore
    PRIVATE
        Qt5::Gui
        KF5::Archive
        KF5::Wallet
        KF5::KIOWidgets
        KF5::I18n
)
if( Qca-qt5_FOUND )
    target_link_libraries(kostore PRIVATE qca-qt5)
endif()

set_target_properties(kostore PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kostore ${INSTALL_TARGETS_DEFAULT_ARGS} )

if (SHOULD_BUILD_DEVEL_HEADERS)

install( FILES
    ${CMAKE_CURRENT_BINARY_DIR}/kostore_export.h
    KoStore.h
DESTINATION ${INCLUDE_INSTALL_DIR}/calligra COMPONENT Devel)

endif()
