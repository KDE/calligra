
include_directories(${CMAKE_BINARY_DIR}/filters/ ${WPD_INCLUDE_DIR} ${LIBWPG_INCLUDE_DIR}/)

set(wpgimport_PART_SRCS WPGImport.cpp)

kde4_add_plugin(wpgimport ${wpgimport_PART_SRCS})

target_link_libraries(wpgimport komain ${LIBWPG_LIBRARIES} ${LIBWPG_STREAM_LIBRARIES} ${WPD_LIBRARIES})

install(TARGETS wpgimport DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES karbon_wpg_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})
install(FILES karbon_wpg_thumbnail.desktop DESTINATION ${SERVICES_INSTALL_DIR})

