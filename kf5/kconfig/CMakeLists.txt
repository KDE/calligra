#cmake_minimum_required(VERSION 2.8.11)

project(KConfig)

if(NOT kdelibs_SOURCE_DIR)

  find_package(ECM 0.0.8 REQUIRED NO_MODULE)
  set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} cmake/)

  find_package(Qt5 5.1.0 REQUIRED NO_MODULE COMPONENTS Widgets Xml Test Concurrent)

#  find_package(KF5 5.0.0 REQUIRED MODULE COMPONENTS CMake Compiler InstallDirs)
endif()

include(FeatureSummary)

include(ECMSetupVersion)
ecm_setup_version(5 0 0 VARIABLE_PREFIX KCONFIG
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kconfig_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KConfigConfigVersion.cmake")


if(WIN32)
    find_package(KDEWin REQUIRED)
    include_directories(${KDEWIN_INCLUDES})
endif()

# Needed for the tests or examples to build correctly
set(KConfig_KCFGC_EXECUTABLE kconfig_compiler)
include(KConfigMacros.cmake)

add_subdirectory(src)
#add_subdirectory(autotests)


# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KConfig")

include(CMakePackageConfigHelpers)

configure_package_config_file("${CMAKE_CURRENT_SOURCE_DIR}/KConfigConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KConfigConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS  INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX )

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KConfigConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KConfigConfigVersion.cmake"
               "${CMAKE_CURRENT_SOURCE_DIR}/KConfigMacros.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel )


install(EXPORT KConfigTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE KConfigTargets.cmake NAMESPACE KF5:: COMPONENT Devel)


install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kconfig_version.h
        DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel )


if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
   feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
