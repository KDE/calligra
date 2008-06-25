
add_subdirectory( kexisql3 )

if(WITH_WEBFORMS)
  macro_optional_add_subdirectory(shttpd)
endif(WITH_WEBFORMS)

#ancient add_subdirectory( kexisql )

