add_subdirectory( tests )

include_directories(
    ${ZLIB_INCLUDE_DIR}
    ../../libmso
    ${KOMAIN_INCLUDES}
    ${KOTEXT_INCLUDES}
)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "SunPro")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS} -KPIC")
else("${CMAKE_CXX_COMPILER_ID}" MATCHES "SunPro")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS} -fPIC")
endif("${CMAKE_CXX_COMPILER_ID}" MATCHES "SunPro")

kde4_add_library(ppttoodplib STATIC PptToOdp.cpp ParsedPresentation.cpp DateTimeFormat.cpp pptstyle.cpp)
target_link_libraries(ppttoodplib koodf mso ${ZLIB_LIBRARIES})

kde4_add_executable(ppttoodp ppttoodpmain.cpp)
target_link_libraries(ppttoodp ppttoodplib)

kde4_add_plugin(calligra_filter_ppt2odp PowerPointImport.cpp)
target_link_libraries(calligra_filter_ppt2odp ppttoodplib komain)

install(TARGETS calligra_filter_ppt2odp  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  calligra_filter_ppt2odp.desktop  DESTINATION ${SERVICES_INSTALL_DIR})

if(SHOULD_BUILD_EXTRA_THUMBNAIL)
    install( FILES  stage_powerpoint_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)
