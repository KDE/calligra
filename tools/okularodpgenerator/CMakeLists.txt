macro_optional_find_package(Okular)

include_directories( ${OKULAR_INCLUDE_DIR} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES})

########### next target ###############

set( okularGenerator_odp_SRCS OkularOdpGenerator.cpp )

kde4_add_plugin( okularGenerator_odp ${okularGenerator_odp_SRCS} )

target_link_libraries( okularGenerator_odp ${OKULAR_LIBRARIES} ${KDE4_KDEUI_LIBS} komain kopageapp )

install( TARGETS okularGenerator_odp DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES libokularGenerator_odp.desktop okularOdp.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( FILES okularApplication_odp.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
