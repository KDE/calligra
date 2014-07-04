
include_directories( ${OKULAR_INCLUDE_DIR} ${KDE4_INCLUDE_DIR} ${QT_INCLUDES} ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${CMAKE_SOURCE_DIR}/words/part)

########### next target ###############

set( okularGenerator_odt_SRCS OkularOdtGenerator.cpp )

kde4_add_plugin( okularGenerator_odt ${okularGenerator_odt_SRCS} )

target_link_libraries( okularGenerator_odt ${OKULAR_LIBRARIES} ${KDE4_KDEUI_LIBS} komain kopageapp kotextlayout wordsprivate )

install( TARGETS okularGenerator_odt DESTINATION ${PLUGIN_INSTALL_DIR} )

########### install files ###############

install( FILES libokularGenerator_odt.desktop okularOdt.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
install( PROGRAMS okularApplication_odt.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
