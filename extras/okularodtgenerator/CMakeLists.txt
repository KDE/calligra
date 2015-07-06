
include_directories( ${KOMAIN_INCLUDES} ${CMAKE_SOURCE_DIR}/words/part)

########### next target ###############

set( okularGenerator_odt_SRCS OkularOdtGenerator.cpp )

add_library(okularGenerator_odt MODULE ${okularGenerator_odt_SRCS} )

target_link_libraries( okularGenerator_odt
    Okular::okularcore
    komain
    kotextlayout
    wordsprivate
)

install( TARGETS okularGenerator_odt DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES libokularGenerator_odt.desktop okularOdt.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( PROGRAMS okularApplication_odt.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
