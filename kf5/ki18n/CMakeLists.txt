cmake_minimum_required(VERSION 2.8.10)

project(KI18n)

find_package(ECM 0.0.8 REQUIRED NO_MODULE)

set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(FeatureSummary)
include(CMakePackageConfigHelpers)
include(ECMSetupVersion)

find_package(KF5 5.0.0 REQUIRED MODULE COMPONENTS CMake Compiler InstallDirs)

find_package(Qt5Test 5.1.0 REQUIRED NO_MODULE)

ecm_setup_version(5 0 0 VARIABLE_PREFIX KI18N
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/ki18n_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KI18nConfigVersion.cmake")

remove_definitions(-DQT_NO_KEYWORDS)
remove_definitions(-DQT_NO_CAST_FROM_ASCII)
remove_definitions(-DQT_NO_CAST_TO_ASCII)
remove_definitions(-DQT_NO_CAST_FROM_BYTEARRAY)

add_subdirectory(src)
#add_subdirectory(autotests)

# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KI18n")

configure_package_config_file("${CMAKE_CURRENT_LIST_DIR}/KI18nConfig.cmake.in"
                              "${CMAKE_CURRENT_BINARY_DIR}/KI18nConfig.cmake"
                              INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS  INCLUDE_INSTALL_DIR LIB_INSTALL_DIR CMAKE_INSTALL_PREFIX)

install(FILES  "${CMAKE_CURRENT_BINARY_DIR}/KI18nConfig.cmake"
               "${CMAKE_CURRENT_BINARY_DIR}/KI18nConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT KI18nTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KI18nTargets.cmake NAMESPACE KF5::)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ki18n_version.h
        DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
    feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
