
find_package(Qt5Widgets 5.2.0 REQUIRED NO_MODULE)

set(libkconfiggui_SRCS
   kconfiggui.cpp
   kconfiggroupgui.cpp
   kconfigskeleton.cpp
   kstandardshortcut.cpp
   kwindowconfig.cpp
)

add_library(KConfigGui ${libkconfiggui_SRCS})
generate_export_header(KConfigGui)

target_link_libraries(KConfigGui Qt5::Widgets KConfigCore)

if(IS_ABSOLUTE "${INCLUDE_INSTALL_DIR}")
  target_include_directories(KConfigGui INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>" )
else()
  target_include_directories(KConfigGui INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}>" )
endif()

set_target_properties(KConfigGui PROPERTIES VERSION   ${KCONFIG_VERSION_STRING}
                                            SOVERSION ${KCONFIG_SOVERSION}
)

install(TARGETS KConfigGui EXPORT KConfigTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install( FILES
   ${CMAKE_CURRENT_BINARY_DIR}/kconfiggui_export.h
   kconfiggui.h
   kconfigskeleton.h
   kstandardshortcut.h
   kwindowconfig.h
   DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)
