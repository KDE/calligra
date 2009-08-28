

include_directories(${CMAKE_BINARY_DIR}/filters/ ${WPD_INCLUDE_DIR} ${LIBWPG_INCLUDE_DIR}/libwpg-0.1)

set(wpgimport_PART_SRCS 
WPGImport.cpp 
OdgExporter.cxx
FileOutputHandler.cxx
GraphicsElement.cxx)

kde4_add_plugin(wpgimport WITH_PREFIX ${wpgimport_PART_SRCS})

target_link_libraries(wpgimport komain ${LIBWPG_LIBRARIES} ${LIBWPG_STREAM_LIBRARIES})

install(TARGETS wpgimport DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES karbon_wpg_import.desktop DESTINATION ${SERVICES_INSTALL_DIR})

