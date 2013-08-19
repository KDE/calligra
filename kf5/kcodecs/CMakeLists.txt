cmake_minimum_required(VERSION 2.8.10)

project(KCodecs)

find_package(ECM 0.0.8 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} )

find_package(KF5 5.0.0 REQUIRED MODULE COMPONENTS CMake Compiler InstallDirs)

remove_definitions(-DQT_NO_KEYWORDS)

include(FeatureSummary)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)

ecm_setup_version(5 0 0 VARIABLE_PREFIX KCODECS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kcodecs_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KCodecsConfigVersion.cmake")

add_subdirectory(src)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KCodecs")

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KCodecsConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KCodecsConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS  INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KCodecsConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KCodecsConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KCodecsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KCodecsTargets.cmake NAMESPACE KF5:: )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kcodecs_version.h
        DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel )


if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
