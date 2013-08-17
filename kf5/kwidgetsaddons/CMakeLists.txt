cmake_minimum_required(VERSION 2.8.10)

project(KWidgetsAddons)

find_package(ECM 0.0.8 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(KF5 5.0 REQUIRED MODULE COMPONENTS CMake Compiler InstallDirs)
remove_definitions(-DQT_NO_KEYWORDS)

find_package(Qt5Widgets 5.1.0 REQUIRED NO_MODULE)

include(FeatureSummary)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)

ecm_setup_version(5 0 0 VARIABLE_PREFIX KWIDGETSADDONS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kwidgetsaddons_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KWidgetsAddonsConfigVersion.cmake")

add_subdirectory(src)
#add_subdirectory(autotests)
#add_subdirectory(tests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KWidgetsAddons")

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KWidgetsAddonsConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KWidgetsAddonsConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS  INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KWidgetsAddonsConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KWidgetsAddonsConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )

install(EXPORT KWidgetsAddonsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KWidgetsAddonsTargets.cmake NAMESPACE KF5:: )

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/kwidgetsaddons_version.h
  DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
