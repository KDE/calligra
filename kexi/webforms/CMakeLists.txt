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
# Model
  model/DataProvider.cpp
  model/Database.cpp
# View
  view/View.cpp
  view/Index.cpp
  view/TemplateProvider.cpp
# Controller
  Controller.cpp
# Other services
  FileService.cpp
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
