add_definitions(-DTRANSLATION_DOMAIN=\"okularGenerator_odp\")

include_directories( ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES})

########### next target ###############

set( okularGenerator_odp_SRCS OkularOdpGenerator.cpp )

add_library(okularGenerator_odp MODULE ${okularGenerator_odp_SRCS} )

target_link_libraries( okularGenerator_odp
    Okular::Core
    komain
    kopageapp
)

install( TARGETS okularGenerator_odp DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES libokularGenerator_odp.desktop okularOdp.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( PROGRAMS okularApplication_odp.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
