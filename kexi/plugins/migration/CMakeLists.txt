
include_directories( ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi ${CMAKE_SOURCE_DIR}/kexi/widget ${CMAKE_SOURCE_DIR}/kexi/migration ${CMAKE_SOURCE_DIR}/kexi/kexiDB ${KDE4_INCLUDES}  )


########### next target ###############

set(kexihandler_migration_PART_SRCS keximigrationpart.cpp )


kde4_add_plugin(kexihandler_migration ${kexihandler_migration_PART_SRCS})



target_link_libraries(kexihandler_migration  ${KDE4_KDECORE_LIBS} kexicore keximigrate )

install(TARGETS kexihandler_migration  DESTINATION ${PLUGIN_INSTALL_DIR})


########### install files ###############

install( FILES  keximigrationhandler.desktop  DESTINATION ${SERVICES_INSTALL_DIR}/kexi)
