#project(kplatowork)

add_subdirectory( templates )
add_subdirectory( pics )
#add_subdirectory( toolbar )
#add_subdirectory( tests )

include_directories(${KPLATO_INCLUDES})


########### KPlatoWork private library ###############

set(kplatoworkfactory_LIB_SRCS
    factory.cpp
    mainwindow.cpp
    part.cpp
    view.cpp
)

kde4_add_library(kplatoworkfactory SHARED ${kplatoworkfactory_LIB_SRCS})

target_link_libraries(kplatoworkfactory kplatokernel kplatomodels kplatoui koguiutils)

set_target_properties(kplatoworkfactory PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatoworkfactory DESTINATION ${LIB_INSTALL_DIR})


########### KPlatoWork part ###############

set(kplatoworkpart_PART_SRCS factoryinit.cpp )


kde4_add_plugin(kplatoworkpart WITH_PREFIX ${kplatoworkpart_PART_SRCS})

target_link_libraries(kplatoworkpart kplatoworkfactory)

install(TARGETS kplatoworkpart DESTINATION ${PLUGIN_INSTALL_DIR})


########### KPlatoWork application ###############

set(kplatoworkapp_LIB_SRCS application.cpp )


kde4_add_library(kplatoworkapp  SHARED ${kplatoworkapp_LIB_SRCS})

target_link_libraries(kplatoworkapp kplatoworkfactory komain)

install(TARGETS kplatoworkapp DESTINATION ${LIB_INSTALL_DIR})


########### KPlatoWork executable ###############

set(kplatowork_KDEINIT_SRCS main.cpp)

kde4_add_kdeinit_executable( kplatowork ${kplatowork_KDEINIT_SRCS})

target_link_libraries(kdeinit_kplatowork kplatoworkapp )

install(TARGETS kdeinit_kplatowork  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kplatowork kdeinit_kplatowork )
install(TARGETS kplatowork  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kplatoworkpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kplatowork.rc kplatowork_readonly.rc kplatowork_mainwindow.rc DESTINATION ${DATA_INSTALL_DIR}/kplatowork)
install( FILES  kplatowork.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})




