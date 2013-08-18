cmake_minimum_required(VERSION 2.8.10)

project(KConfigWidgets)

if(NOT kdelibs_SOURCE_DIR)

  find_package(ECM 0.0.8 REQUIRED NO_MODULE)

  set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

  find_package(Qt5 5.1.0 REQUIRED NO_MODULE COMPONENTS Widgets)

  find_package(KF5 5.0.0 REQUIRED MODULE COMPONENTS CMake Compiler InstallDirs KWidgetsAddons KConfig)

endif()

include_directories("../kconfig/src/core" 
                    "../kconfig/src/gui"
                    "../kwidgetsaddons/src/"
                    ${CMAKE_CURRENT_BINARY_DIR}/../kconfig/src/gui
                    ${CMAKE_CURRENT_BINARY_DIR}/../kconfig/src/core
                    ${CMAKE_CURRENT_BINARY_DIR}/../kwidgetsaddons/src/
)

include(FeatureSummary)

include(ECMSetupVersion)

ecm_setup_version(5 0 0 VARIABLE_PREFIX KCONFIGWIDGETS
                        VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kconfigwidgets_version.h"
                        PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KConfigWidgetsConfigVersion.cmake")

remove_definitions(-DQT_NO_KEYWORDS -DQT_NO_CAST_FROM_ASCII -DQT_NO_CAST_FROM_BYTEARRAY)

add_subdirectory(src)
#add_subdirectory(tests)
#add_subdirectory(autotests)


# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${CMAKECONFIG_INSTALL_PREFIX}/KConfigWidgets")

include(CMakePackageConfigHelpers)

configure_package_config_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/KConfigWidgetsConfig.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/KConfigWidgetsConfig.cmake"
  INSTALL_DESTINATION  ${CMAKECONFIG_INSTALL_DIR}
  PATH_VARS  INCLUDE_INSTALL_DIR CMAKE_INSTALL_PREFIX
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/KConfigWidgetsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/KConfigWidgetsConfigVersion.cmake"
  DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
  COMPONENT Devel
)

install(EXPORT KConfigWidgetsTargets DESTINATION "${CMAKECONFIG_INSTALL_DIR}" FILE KConfigWidgetsTargets.cmake NAMESPACE KF5:: )

install(FILES
   ${CMAKE_CURRENT_BINARY_DIR}/kconfigwidgets_version.h
  DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
endif()
