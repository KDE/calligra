project(webforms)

add_definitions(-DEMBEDDED) # for shttpd

include_directories(${CMAKE_SOURCE_DIR}/kexi/3rdparty/shttpd/src)

set(webforms_SRCS
  main.cpp
  Server.cpp
  ServerConfig.cpp
  DataProvider.cpp
)

add_executable(kwebforms ${webforms_SRCS})
target_link_libraries(kwebforms shttpd ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}
  kexidb kexicore keximain
)
install(TARGETS kwebforms DESTINATION ${BIN_INSTALL_DIR})
