project(webforms)

add_definitions(-DEMBEDDED) # for shttpd

include_directories(${CMAKE_SOURCE_DIR}/kexi/3rdparty/shttpd/src
  ${GOOGLE_CTEMPLATE_INCLUDE_DIR})

set(webforms_SRCS
  main.cpp
  Server.cpp
  ServerConfig.cpp
  HTTPStream.cpp
  DataProvider.cpp
  TemplateProvider.cpp
  Request.cpp
# Index view
  Index.cpp
# CRUD Architecture
  Handler.cpp
# CRUD
  Create.cpp
  Read.cpp
  Update.cpp
  Delete.cpp
)

kde4_add_executable(kwebforms NOGUI ${webforms_SRCS})
target_link_libraries(kwebforms kexi_shttpd ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}
  ${GOOGLE_CTEMPLATE_THREAD_LIB}
  kexidb kexicore keximain
)
install(TARGETS kwebforms ${INSTALL_TARGETS_DEFAULT_ARGS})
