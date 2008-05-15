project(webforms)

add_definitions(-DEMBEDDED)
remove_definitions(-DQT_NO_STL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/shttpd/src)

set(webforms_SRCS
  main.cpp
  Server.cpp
  DataProvider.cpp)

add_executable(kwebforms ${webforms_SRCS})
target_link_libraries(kwebforms shttpd ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}
  kexidb
)
install(TARGETS kwebforms DESTINATION ${BIN_INSTALL_DIR})
