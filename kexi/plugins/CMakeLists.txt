add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44021)

add_subdirectory( reports )

if (KEXI_MOBILE)

else (KEXI_MOBILE)
  add_subdirectory( tables )
  add_subdirectory( queries )
  add_subdirectory( forms )
  add_subdirectory( migration )
  add_subdirectory( importexport )

  if(SHOULD_BUILD_SCRIPTING)
     add_subdirectory(scripting)
  endif(SHOULD_BUILD_SCRIPTING)

endif (KEXI_MOBILE)

#TODO add_subdirectory( relations )
#TODO add_subdirectory(macros)
