
add_subdirectory( kexisql3 )

if(DEFINED WITH_WEBFORMS)
  add_subdirectory(shttpd)
endif(DEFINED WITH_WEBFORMS)

#ancient add_subdirectory( kexisql )

