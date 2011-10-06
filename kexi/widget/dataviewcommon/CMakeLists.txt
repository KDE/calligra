

include_directories( ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/libs ${KDE4_INCLUDES}  )


########### next target ###############

set(kexidataviewcommon_LIB_SRCS 
   kexidataawareobjectiface.cpp
   kexidataawarepropertyset.cpp
   kexidataawareview.cpp
   kexidataprovider.cpp
   kexitableviewdata.cpp
   KexiTableViewColumn.cpp
   kexiformdataiteminterface.cpp
)


kde4_add_library(kexidataviewcommon SHARED ${kexidataviewcommon_LIB_SRCS})

target_link_libraries(
    kexidataviewcommon
    kexicore
    kexiutils
    kexiguiutils
    ${KOPROPERTY_LIBS}
    kexidb
    ${KDE4_KIO_LIBS}
    ${QT_QT3SUPPORT_LIBRARY}
)

set_target_properties(kexidataviewcommon PROPERTIES VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION} )
install(TARGETS kexidataviewcommon  ${INSTALL_TARGETS_DEFAULT_ARGS})
