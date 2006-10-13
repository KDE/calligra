

include_directories(  ${CMAKE_BINARY_DIR}/filters/)


########### next target ###############

set(kritagmagickimport_PART_SRCS magickimport.cpp kis_image_magick_converter.cc )

kde4_automoc(kritagmagickimport ${kritagmagickimport_PART_SRCS})

kde4_add_plugin(kritagmagickimport WITH_PREFIX ${kritagmagickimport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritagmagickimport )

target_link_libraries(kritagmagickimport kritaui ${GRAPHICSMAGICK_LIBRARIES})

install(TARGETS kritagmagickimport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### next target ###############

set(kritagmagickexport_PART_SRCS magickexport.cpp kis_image_magick_converter.cc )

kde4_automoc(kritagmagickexport ${kritagmagickexport_PART_SRCS})

kde4_add_plugin(kritagmagickexport WITH_PREFIX ${kritagmagickexport_PART_SRCS})

kde4_install_libtool_file( ${PLUGIN_INSTALL_DIR} kritagmagickexport )

target_link_libraries(kritagmagickexport kritaui ${GRAPHICSMAGICK_LIBRARIES})

install(TARGETS kritagmagickexport  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  krita_magick_import.desktop krita_magick_export.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
install( FILES  krita_magick.desktop  DESTINATION ${XDG_APPS_DIR})

SET_TARGET_PROPERTIES( kritagmagickexport kritagmagickimport PROPERTIES COMPILE_FLAGS ${GRAPHICSMAGICK_COMPILE_FLAGS})

