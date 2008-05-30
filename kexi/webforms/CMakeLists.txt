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
  IndexView.cpp
  TableView.cpp
  UpdateView.cpp
  Request.cpp
)

kde4_add_executable(kwebforms ${webforms_SRCS})
target_link_libraries(kwebforms shttpd ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}
  ${GOOGLE_CTEMPLATE_THREAD_LIB}
  kexidb kexicore keximain
)
install(TARGETS kwebforms ${INSTALL_TARGETS_DEFAULT_ARGS})
