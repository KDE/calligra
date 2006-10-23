

include_directories( ${CMAKE_SOURCE_DIR}/lib/kformula ${CMAKE_SOURCE_DIR}/kword ${KOTEXT_INCLUDES} ${KOFFICEUI_INCLUDES} )


########### next target ###############

set(kwmailmerge_kabc_PART_SRCS KWMailMergeKABC.cpp KWMailMergeKABCConfig.cpp )

kde4_automoc(${kwmailmerge_kabc_PART_SRCS})

kde4_add_ui3_files(kwmailmerge_kabc_PART_SRCS addresspicker.ui )

kde4_add_plugin(kwmailmerge_kabc ${kwmailmerge_kabc_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kwmailmerge_kabc )

target_link_libraries(kwmailmerge_kabc ${KDE4_KDEUI_LIBS} kwmailmerge_interface ${KDE4_KABC_LIBS} )

install(TARGETS kwmailmerge_kabc  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  kwmailmerge_kabc.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

