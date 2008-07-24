# NOTE: Is this really needed?
project(kwebforms)

include_directories(
    ${CMAKE_SOURCE_DIR}/kexi/3rdparty/shttpd/src
    ${GOOGLE_CTEMPLATE_INCLUDE_DIR}
    ${PionNet_INCLUDE_DIR}
)

# Workaround, exceptions are needed for pion::net
# FIXME: This won't work for other compilers
add_definitions(-fexceptions)

set(webforms_SRCS
  DataProvider.cpp
  TemplateProvider.cpp
  WebFormsService.cpp
# Model
  model/Database.cpp
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
  BlobService.cpp
# Authentication framework
  auth/Authenticator.cpp
  auth/User.cpp
# Main
  main.cpp
)

kde4_add_executable(kwebforms NOGUI ${webforms_SRCS})

target_link_libraries(kwebforms
    ${KDE4_KDECORE_LIBS}
    ${KDE4_KDEUI_LIBS}
    ${GOOGLE_CTEMPLATE_THREAD_LIB}
    ${PionNet_LIBRARY}
    kexidb
    kexicore
    keximain
)

install(TARGETS kwebforms ${INSTALL_TARGETS_DEFAULT_ARGS})
