
find_package(Qt5Core 5.1.0 REQUIRED NO_MODULE)

set(libkconfigcore_SRCS
   kconfig.cpp
   kconfigbase.cpp
   kconfigdata.cpp
   kconfiggroup.cpp
   kconfigbackend.cpp
   kconfigini.cpp
   kdesktopfile.cpp
   ksharedconfig.cpp
   kcoreconfigskeleton.cpp
   kcoreauthorized.cpp
)

add_library(KConfigCore ${libkconfigcore_SRCS})
generate_export_header(KConfigCore)

target_link_libraries(KConfigCore Qt5::Core)
if(WIN32)
    target_link_libraries(KConfigCore ${KDEWIN_LIBRARIES})
endif()

if(IS_ABSOLUTE "${INCLUDE_INSTALL_DIR}")
  target_include_directories(KConfigCore INTERFACE "$<INSTALL_INTERFACE:${INCLUDE_INSTALL_DIR}>" )
else()
  target_include_directories(KConfigCore INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/${INCLUDE_INSTALL_DIR}>" )
endif()

set_target_properties(KConfigCore PROPERTIES VERSION   ${KCONFIG_VERSION_STRING}
                                             SOVERSION ${KCONFIG_SOVERSION} )

install(TARGETS KConfigCore EXPORT KConfigTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install( FILES
   ${CMAKE_CURRENT_BINARY_DIR}/kconfigcore_export.h
   conversion_check.h
   kconfig.h
   #kconfigbackend.h re-enable post-API review and implementation (4.2?)
   kconfigbase.h
   kconfiggroup.h
   kdesktopfile.h
   ksharedconfig.h
   kcoreconfigskeleton.h
   kcoreauthorized.h
   ksharedptr.h
   DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)
