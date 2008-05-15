
add_subdirectory( kexisql3 )

if(KEXIWEBFORMS_SUPPORTED)
  macro_optional_add_subdirectory(shttpd)
endif(KEXIWEBFORMS_SUPPORTED)

#ancient add_subdirectory( kexisql )

