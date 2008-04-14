set(shttpd_SRCS string.c
  shttpd.c
  log.c
  auth.c
  md5.c
  cgi.c
  config.c
  io_file.c
  io_socket.c
  io_ssl.c
  io_emb.c
  io_dir.c)

if(WIN32)
  list(APPEND shttpd_SRCS compat_win32.c)
else(WIN32)
  if (UNIX)
    list(APPEND shttpd_SRCS compat_unix.c)
  endif(UNIX)
endif(WIN32)

# Disable unneeded features
add_definitions(-DNDEBUG -DNO_CGI -DNO_SSI)

add_library(shttpd ${shttpd_SRCS})
