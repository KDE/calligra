project(webforms)

add_definitions(-DEMBEDDED) # for shttpd

include_directories(${CMAKE_SOURCE_DIR}/kexi/3rdparty/shttpd/src
  ${GOOGLE_CTEMPLATE_INCLUDE_DIR})

# Workaround, exceptions are needed for pion::net
add_definitions(-fexceptions -Wno-deprecated)

set(webforms_SRCS
  main.cpp
  DataProvider.cpp
  TemplateProvider.cpp
# Pattern Server
  PatternServer.cpp
  WebFormsService.cpp
# Index view
  IndexService.cpp
# CRUD
  CreateService.cpp
  ReadService.cpp
  UpdateService.cpp
  DeleteService.cpp
# Other services
  FileService.cpp
  QueryService.cpp
)

kde4_add_executable(kwebforms NOGUI ${webforms_SRCS})
target_link_libraries(kwebforms kexi_shttpd ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}
  ${GOOGLE_CTEMPLATE_THREAD_LIB}
  kexidb kexicore keximain pion-net
)
install(TARGETS kwebforms ${INSTALL_TARGETS_DEFAULT_ARGS})
