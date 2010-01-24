include_directories(${ZLIB_INCLUDE_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fPIC")

kde4_add_library(ppttoodplib STATIC PptToOdp.cpp ParsedPresentation.cpp DateTimeFormat.cpp pictures.cpp pole.cpp mso/simpleParser.cpp)
target_link_libraries(ppttoodplib komain ${ZLIB_LIBRARIES})

kde4_add_executable(ppttoodp ppttoodpmain.cpp)
target_link_libraries(ppttoodp ppttoodplib)

kde4_add_plugin(powerpointimport WITH_PREFIX PowerPointImport.cpp)
target_link_libraries(powerpointimport ppttoodplib)

install(TARGETS powerpointimport  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  kpresenter_powerpoint_import.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

