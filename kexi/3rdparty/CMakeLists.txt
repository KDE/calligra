
add_subdirectory( kexisql3 )

if(DEFINED BUILD_WITH_KWFD)
  add_subdirectory(shttpd)
endif(DEFINED BUILD_WITH_KWFD)

#ancient add_subdirectory( kexisql )

