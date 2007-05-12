

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg ${CMAKE_BINARY_DIR}/filters/)


set(libwpg_SRCS 
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPG1Parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGBrush.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGGradient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGOLEStream.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGPen.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGraphics.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGStreamImplementation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGSVGGenerator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPG2Parser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGColor.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGHeader.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGPath.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGPoint.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGRect.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGString.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/../libwpg/WPGXParser.cpp
)


set(wpgimport_PART_SRCS WPGImport.cpp ${libwpg_SRCS})

kde4_automoc(${wpgimport_PART_SRCS})

kde4_add_plugin(wpgimport WITH_PREFIX ${wpgimport_PART_SRCS})



target_link_libraries(wpgimport komain)

install(TARGETS wpgimport  DESTINATION ${PLUGIN_INSTALL_DIR})


install( FILES  karbon_wpg_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})



