
project(kplato)

if(KDE4_BUILD_TESTS)
    # only with this definition will the KPLATO_TEST_EXPORT macro do something
    add_definitions(-DCOMPILING_TESTS)
endif(KDE4_BUILD_TESTS)

set( KPLATO_INCLUDES
    ${CMAKE_SOURCE_DIR}/kplato/libs/kernel
    ${CMAKE_SOURCE_DIR}/kplato/libs/models
    ${CMAKE_BINARY_DIR}/kplato/libs/models
    ${CMAKE_SOURCE_DIR}/kplato/libs/ui
    ${CMAKE_BINARY_DIR}/kplato/libs/ui
    ${CMAKE_SOURCE_DIR}/kdgantt
    ${CMAKE_BINARY_DIR}/kdgantt
    ${KOGUIUTILS_INCLUDES}
)

add_subdirectory( libs )

add_subdirectory( templates )
add_subdirectory( pics )
add_subdirectory( reports )
add_subdirectory( toolbar )
add_subdirectory( plugins )
#add_subdirectory( tests )

add_subdirectory( workpackage )

include_directories(${KPLATO_INCLUDES})

########### KPlato private library ###############

set(kplatoprivate_LIB_SRCS
    kptviewlistdocker.cpp
    kptviewlist.cpp
    kptviewlistdialog.cpp
    kptviewlistcommand.cpp
    
    kptconfigdialog.cpp
    kptconfig.cpp
    kpttaskdefaultpanel.cpp

    kptwpcontroldialog.cpp
    kptwpcontrolpanel.cpp
    
    kptcontext.cpp

    kptfactory.cpp
    kptpart.cpp
    kptview.cpp
    KPtViewAdaptor.cpp
)

kde4_add_ui_files(kplatoprivate_LIB_SRCS
    kptviewlistaddview.ui
    kptviewlisteditview.ui
    kptviewlisteditcategory.ui

    kptconfigtaskpanelbase.ui
)

kde4_add_library(kplatoprivate SHARED ${kplatoprivate_LIB_SRCS} ${libkdgantt_SRCS} )

target_link_libraries(kplatoprivate kplatokernel kplatomodels kplatoui koguiutils  )

set_target_properties(kplatoprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatoprivate DESTINATION ${LIB_INSTALL_DIR})

########### KPlato part ###############

set(kplatopart_PART_SRCS kptfactoryinit.cpp )


kde4_add_plugin(kplatopart WITH_PREFIX ${kplatopart_PART_SRCS})

target_link_libraries(kplatopart kplatoprivate)

install(TARGETS kplatopart DESTINATION ${PLUGIN_INSTALL_DIR})


########### KPlato executable ###############

set(kplato_KDEINIT_SRCS main.cpp )

kde4_add_kdeinit_executable( kplato ${kplato_KDEINIT_SRCS})

target_link_libraries(kdeinit_kplato komain )

install(TARGETS kdeinit_kplato  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kplato kdeinit_kplato )
install(TARGETS kplato  ${INSTALL_TARGETS_DEFAULT_ARGS})

########### install files ###############

install( FILES  kplatopart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kplato.rc kplato_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/kplato)
install( FILES  kplato.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install( FILES  kplatorc DESTINATION ${CONFIG_INSTALL_DIR})

