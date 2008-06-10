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
  Request.cpp
# Index view
  Index.cpp
# CRUD
  Read.cpp
  Update.cpp
  Delete.cpp
)

# This target is needed for emacs' flymake
add_custom_target(check-syntax)
add_dependencies(check-syntax kwebforms/fast)

kde4_add_executable(kwebforms ${webforms_SRCS})
target_link_libraries(kwebforms shttpd ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS}
  ${GOOGLE_CTEMPLATE_THREAD_LIB}
  kexidb kexicore keximain
)
install(TARGETS kwebforms ${INSTALL_TARGETS_DEFAULT_ARGS})
