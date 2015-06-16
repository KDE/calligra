include_directories( ${XBase_INCLUDE_DIR} ${CMAKE_SOURCE_DIR}/kexi   )

########### next target ###############

set(keximigrate_xbase_PART_SRCS xbasemigrate.cpp )

add_library(keximigrate_xbase MODULE ${keximigrate_xbase_PART_SRCS})


target_link_libraries(
    keximigrate_xbase
    keximigrate
    kexiutils

    KDb

    ${XBase_LIBRARIES}
)

install(TARGETS keximigrate_xbase DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

install(TARGETS keximigrate_xbase DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
