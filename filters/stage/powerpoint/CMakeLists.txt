add_subdirectory( tests )

include_directories(
    ${ZLIB_INCLUDE_DIR}
    ../../libmso
    ${KOMAIN_INCLUDES}
    ${KOTEXT_INCLUDES}
)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "SunPro")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS} -KPIC")
else()
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS} -fPIC")
endif()

kde4_add_library(ppttoodplib STATIC PptToOdp.cpp ParsedPresentation.cpp DateTimeFormat.cpp pptstyle.cpp)
target_link_libraries(ppttoodplib koodf mso ${ZLIB_LIBRARIES})

kde4_add_executable(ppttoodp ppttoodpmain.cpp)
target_link_libraries(ppttoodp ppttoodplib)

kde4_add_plugin(calligra_filter_ppt2odp PowerPointImport.cpp)
target_link_libraries(calligra_filter_ppt2odp ppttoodplib komain)

install(TARGETS calligra_filter_ppt2odp  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES  calligra_filter_ppt2odp.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/calligra)

if(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    install( FILES  stage_powerpoint_thumbnail.desktop  DESTINATION ${SERVICES_INSTALL_DIR})
endif()

if(SHOULD_BUILD_OKULARGENERATOR_PPT)
    install( FILES libokularGenerator_powerpoint_calligra.desktop okularPowerpoint_calligra.desktop DESTINATION ${SERVICES_INSTALL_DIR} )
    install( PROGRAMS okularApplication_powerpoint_calligra.desktop DESTINATION ${XDG_APPS_INSTALL_DIR} )
endif()
