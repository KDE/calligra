#project(kplatowork)

add_subdirectory( templates )
#add_subdirectory( pics )
#add_subdirectory( toolbar )
#add_subdirectory( tests )

include_directories(${KPLATO_INCLUDES})

########### KPlatoWork private library ###############

set(kplatoworkprivate_LIB_SRCS
    factory.cc
    part.cc
    view.cc
)

kde4_add_ui_files(kplatoworkprivate_LIB_SRCS
)

kde4_add_library(kplatoworkprivate SHARED ${kplatoworkprivate_LIB_SRCS})

target_link_libraries(kplatoworkprivate koguiutils kplatokernel)

set_target_properties(kplatoworkprivate PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )

install(TARGETS kplatoworkprivate DESTINATION ${LIB_INSTALL_DIR})

########### KPlatoWork part ###############

set(kplatoworkpart_PART_SRCS factoryinit.cc )


kde4_add_plugin(kplatoworkpart WITH_PREFIX ${kplatoworkpart_PART_SRCS})

target_link_libraries(kplatoworkpart kplatoworkprivate)

install(TARGETS kplatoworkpart DESTINATION ${PLUGIN_INSTALL_DIR})


########### KPlatoWork executable ###############

set(kplatowork_KDEINIT_SRCS main.cc )

kde4_add_kdeinit_executable( kplatowork ${kplatowork_KDEINIT_SRCS})

target_link_libraries(kdeinit_kplatowork komain )

install(TARGETS kdeinit_kplatowork  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kplatowork kdeinit_kplatowork )
install(TARGETS kplatowork  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kplatoworkpart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  kplatowork.rc kplatowork_readonly.rc DESTINATION ${DATA_INSTALL_DIR}/kplatowork)
install( FILES  kplatowork.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})




