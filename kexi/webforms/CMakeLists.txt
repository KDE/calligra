project(webforms)

add_definitions(-DNO_CGI -DNO_SSI)
remove_definitions(-DQT_NO_STL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/shttpd/src)

set(webforms_SRCS
  main.cpp
  Server.cpp)

add_executable(kwebforms ${webforms_SRCS})
target_link_libraries(kwebforms shttpd ${KDE4_KDECORE_LIBS})
