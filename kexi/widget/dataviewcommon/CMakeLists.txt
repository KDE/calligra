

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs   )


########### next target ###############

set(kexidataviewcommon_LIB_SRCS
   kexidataawareobjectiface.cpp
   kexidataawarepropertyset.cpp
   kexidataawareview.cpp
   kexidataprovider.cpp
   kexiformdataiteminterface.cpp
)

add_library(kexidataviewcommon SHARED ${kexidataviewcommon_LIB_SRCS})

generate_export_header(kexidataviewcommon)

target_link_libraries(
    kexidataviewcommon
    kexicore
    kexiutils
    kexiguiutils

    KDb
    KProperty

    KF5::KIOWidgets
    KF5::XmlGui #KActionCollection...
)

set_target_properties(kexidataviewcommon PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kexidataviewcommon  ${INSTALL_TARGETS_DEFAULT_ARGS})
