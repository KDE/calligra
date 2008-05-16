
add_subdirectory( kexisql3 )

if(KEXIWEBFORMS_SUPPORTED)
  add_subdirectory(shttpd)
endif(KEXIWEBFORMS_SUPPORTED)

#ancient add_subdirectory( kexisql )

