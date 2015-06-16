
include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/migration)


########### next target ###############

set(kexihandler_migration_PART_SRCS keximigrationpart.cpp )


add_library(kexihandler_migration MODULE ${kexihandler_migration_PART_SRCS})



target_link_libraries(kexihandler_migration   kexicore keximigrate )

install(TARGETS kexihandler_migration  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})

install(TARGETS kexihandler_migration  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
