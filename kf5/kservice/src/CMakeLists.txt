include(CheckSymbolExists)
include(CheckFunctionExists)
check_function_exists(mmap HAVE_MMAP)
check_symbol_exists(posix_madvise "sys/mman.h" HAVE_MADVISE)
configure_file(config-ksycoca.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-ksycoca.h )

set(kservice_SRCS
   #kdeinit/ktoolinvocation.cpp
   #services/kautostart.cpp
   services/kmimetypefactory.cpp
   services/kmimetypetrader.cpp
   services/kservice.cpp
   services/kserviceaction.cpp
   services/kservicefactory.cpp
   services/kservicegroup.cpp
   services/kservicegroupfactory.cpp
   services/kserviceoffer.cpp
   services/kservicetype.cpp
   services/kservicetypefactory.cpp
   services/kservicetypeprofile.cpp
   services/kservicetypetrader.cpp
   services/ktraderparse.cpp
   services/ktraderparsetree.cpp
   services/yacc.c
   services/lex.c
   services/kplugininfo.cpp
   sycoca/ksycoca.cpp
   sycoca/ksycocadict.cpp
   sycoca/ksycocaentry.cpp
   sycoca/ksycocafactory.cpp
   sycoca/kmemfile.cpp
   plugin/klibrary.cpp
   plugin/kpluginfactory.cpp
   plugin/kpluginloader.cpp
)
if (WIN32)
   LIST(APPEND kservice_SRCS
      #kdeinit/ktoolinvocation_win.cpp
   )
endif()
if (UNIX)
   LIST(APPEND kservice_SRCS
      #kdeinit/ktoolinvocation_x11.cpp
   )
endif()

add_library(KService ${kservice_SRCS})
generate_export_header(KService)
set(kservice_includes
   ${CMAKE_CURRENT_BINARY_DIR}/.. # Since we publicly include kservice_version.h
   ${CMAKE_CURRENT_SOURCE_DIR}/services
   ${CMAKE_CURRENT_SOURCE_DIR}/sycoca
   ${CMAKE_CURRENT_SOURCE_DIR}/plugin
   #${CMAKE_CURRENT_SOURCE_DIR}/kdeinit
)
target_include_directories(KService PUBLIC "$<BUILD_INTERFACE:${kservice_includes}>")

target_link_libraries(KService
  LINK_PUBLIC
    KConfigCore
#    KCoreAddons
  LINK_PRIVATE
#    KI18n
#    KDBusAddons   # KDEInitInterface
#    Qt5::DBus
     Qt5::Core
)

set_target_properties(KService PROPERTIES VERSION ${KSERVICE_VERSION_STRING}
                                        SOVERSION ${KSERVICE_SOVERSION})

install(TARGETS KService EXPORT KServiceTargets ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
   "${CMAKE_CURRENT_BINARY_DIR}/kservice_export.h"
   #kdeinit/ktoolinvocation.h
   services/kautostart.h
   services/kmimetypetrader.h
   services/kservice.h
   services/kserviceaction.h
   services/kservicegroup.h
   #services/kservicefactory.h: do not install, internal API
   services/kservicetype.h
   #services/kservicetypefactory.h: do not install, internal API
   services/kservicetypeprofile.h
   services/kservicetypetrader.h
   #services/kserviceoffer.h: do not install, internal API
   services/kplugininfo.h
   sycoca/ksycoca.h
   sycoca/ksycocaentry.h
   sycoca/ksycocatype.h
   plugin/klibrary.h
   plugin/kpluginfactory.h
   plugin/kpluginloader.h
   plugin/kexportplugin.h
   DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)

install(FILES
   services/kplugininfo.desktop
   DESTINATION ${SERVICETYPES_INSTALL_DIR}
)

generate_export_header(KService)

#add_subdirectory(desktoptojson)
