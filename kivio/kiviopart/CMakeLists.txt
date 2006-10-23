

add_subdirectory( config )
add_subdirectory( ui )
add_subdirectory( kiviosdk )
add_subdirectory( tools )
add_subdirectory( pics )
add_subdirectory( tiles )
add_subdirectory( stencils )

include_directories( ${CMAKE_SOURCE_DIR}/lib/kformula ${KDE4_INCLUDES}  )


########### next target ###############

set(kiviocommon_LIB_SRCS 
   kivio_canvas.cpp 
   kivio_doc.cpp 
   kivio_factory.cpp 
   kivio_view.cpp 
   kivio_map.cpp 
   kivio_page.cpp 
   kivio_dlg_pageshow.cpp 
   kivio_zoomaction.cpp 
   kivio_icon_view.cpp 
   kivio_spawner_drag.cpp 
   kivio_intra_stencil_data.cpp 
   kivio_stackbar.cpp 
   stencilbarbutton.cpp 
   stencilbardockmanager.cpp 
   kivio_grid_data.cpp 
   kivio_stencil_geometry_panel.cpp 
   KIvioViewIface.cc 
   KIvioDocIface.cc 
   KIvioMapIface.cc 
   KIvioPageIface.cc 
   kivio_command.cc 
   KIvioLayerIface.cc 
   kivioglobal.cpp 
   kiviostencilsetaction.cpp 
   kivioarrowheadaction.cpp 
   KIvioStencilIface.cc 
   kiviodragobject.cpp 
   kiviostencilsetinstaller.cpp 
   addstenciltool.cpp )

kde4_automoc(${kiviocommon_LIB_SRCS})

kde4_add_dcop_skels(kiviocommon_LIB_SRCS 
   KIvioViewIface.h 
   KIvioDocIface.h 
   KIvioMapIface.h 
   KIvioPageIface.h 
   KIvioLayerIface.h 
   KIvioStencilIface.h )

kde4_add_library(kiviocommon SHARED ${kiviocommon_LIB_SRCS})

target_link_libraries(kiviocommon  ${KDE4_KDECORE_LIBS} tools kiviosdk kivioconfig ui kofficeui kopainter kotext )

set_target_properties(kiviocommon PROPERTIES VERSION ${GENERIC_KOFFICE_LIB_VERSION} SOVERSION ${GENERIC_KOFFICE_LIB_SOVERSION} )
install(TARGETS kiviocommon  DESTINATION ${LIB_INSTALL_DIR})


########### next target ###############

set(kiviopart_PART_SRCS kivio_part_init.cc )

kde4_automoc(${kiviopart_PART_SRCS})

kde4_add_plugin(kiviopart WITH_PREFIX ${kiviopart_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kiviopart )

target_link_libraries(kiviopart  ${KDE4_KDECORE_LIBS} kiviocommon )

install(TARGETS kiviopart  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kivio_KDEINIT_SRCS main.cpp )

kde4_automoc(${kivio_KDEINIT_SRCS})

kde4_add_kdeinit_executable( kivio ${kivio_KDEINIT_SRCS})

target_link_libraries(kdeinit_kivio  ${KDE4_KDECORE_LIBS} kofficecore )

install(TARGETS kdeinit_kivio  DESTINATION ${LIB_INSTALL_DIR})

target_link_libraries( kivio kdeinit_kivio )
install(TARGETS kivio  DESTINATION ${BIN_INSTALL_DIR})

########### install files ###############

install( FILES  kivio.desktop  DESTINATION ${XDG_APPS_DIR})
install( FILES  kivio.rc kivio_readonly.rc  DESTINATION ${DATA_INSTALL_DIR}/kivio)
install( FILES  kiviopart.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

kde4_install_icons( ${ICON_INSTALL_DIR}   )
