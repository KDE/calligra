add_subdirectory(visualimagecompare)
add_subdirectory(cstrunner)

configure_file(config_cstester.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config_cstester.h)

include_directories(
    ${KOMAIN_INCLUDES}
    ${KOPAGEAPP_INCLUDES}
    ${CMAKE_SOURCE_DIR}/sheets
    ${CMAKE_SOURCE_DIR}/words/part
    ${CMAKE_SOURCE_DIR}/karbon/ui
)

set(cstester_SRCS
    CSThumbProviderStage.cpp
    CSThumbProviderTables.cpp
    CSThumbProviderWords.cpp
    cstester.cpp )

if(SHOULD_BUILD_KARBON)
    set(cstester_SRCS ${cstester_SRCS}
        CSThumbProviderKarbon.cpp)
endif()

add_executable(cstester ${cstester_SRCS})

target_link_libraries(cstester komain kopageapp koplugin calligrasheetspartlib wordsprivate)
if(SHOULD_BUILD_KARBON)
    target_link_libraries(cstester karbonui)
endif()

install(TARGETS cstester  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

