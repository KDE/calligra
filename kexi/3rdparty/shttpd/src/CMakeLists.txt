add_definitions(-DNO_CGI -DNO_SSI)

set(kexi_shttpd_SRCS string.c
  shttpd.c
  log.c
  auth.c
  md5.c
  config.c
  io_file.c
  io_socket.c
  io_ssl.c
  io_emb.c
  io_dir.c
#  cgi.c
#  io_ssi.c
)

if(WIN32)
  list(APPEND kexi_shttpd_SRCS compat_win32.c shttpd.def)
  set(kexi_shttpd_LIBS ws2_32.lib)
endif(WIN32)

if (UNIX)
  list(APPEND kexi_shttpd_SRCS compat_unix.c)
endif(UNIX)

add_library(kexi_shttpd SHARED ${kexi_shttpd_SRCS})
target_link_libraries(kexi_shttpd ${kexi_shttpd_LIBS} ${CMAKE_DL_LIBS})
set_target_properties(kexi_shttpd PROPERTIES VERSION 1.3.9 SOVERSION 1 )
install(TARGETS kexi_shttpd DESTINATION ${LIB_INSTALL_DIR})
