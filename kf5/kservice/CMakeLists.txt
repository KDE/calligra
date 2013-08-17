project(KService)

include(ECMSetupVersion)

ecm_setup_version(5 0 0 VARIABLE_PREFIX KSERVICE
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kservice_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KServiceConfigVersion.cmake")


# TODO: Remove these
remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_STRICT_ITERATORS)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)
remove_definitions(-DQT_USE_FAST_CONCATENATION)
remove_definitions(-DQT_USE_FAST_OPERATOR_PLUS)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KService")

include(CMakePackageConfigHelpers)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KServiceConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KServiceConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS  INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX
                             )

install(EXPORT KServiceTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KServiceTargets.cmake NAMESPACE KF5:: )

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/KServiceConfig.cmake"
              "${CMAKE_CURRENT_BINARY_DIR}/KServiceConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel
       )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kservice_version.h
        DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel )

add_subdirectory(src)
