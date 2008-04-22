project(webforms)

add_definitions(-DNO_CGI -DNO_SSI)
remove_definitions(-DQT_NO_STL)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../3rdparty/shttpd/src)

set(webforms_SRCS
  main.cpp
  HttpServer.cpp)

add_executable(wfd ${webforms_SRCS})
target_link_libraries(wfd shttpd ${KDE4_KDECORE_LIBS})
