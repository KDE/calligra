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

add_library(planstore SHARED ${kostore_LIB_SRCS})
generate_export_header(planstore BASE_NAME kostore)

target_link_libraries(planstore
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
    target_link_libraries(planstore PRIVATE qca-qt5)
endif()

set_target_properties(planstore PROPERTIES
    VERSION ${GENERIC_PLAN_LIB_VERSION} SOVERSION ${GENERIC_PLAN_LIB_SOVERSION}
)
install(TARGETS planstore ${INSTALL_TARGETS_DEFAULT_ARGS} )
