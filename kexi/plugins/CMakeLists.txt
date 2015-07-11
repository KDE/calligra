add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44021)

if (KEXI_MOBILE)

else ()
  add_subdirectory( tables )
  add_subdirectory( queries )
  add_subdirectory( forms )
  # KEXI3 TODO add_subdirectory( reports )
  # KEXI3 TODO add_subdirectory( migration )
  add_subdirectory( importexport )

  if(SHOULD_BUILD_FEATURE_SCRIPTING AND KEXI_SCRIPTS_SUPPORT)
     # KEXI3 TODO add_subdirectory(scripting)
  endif()
endif ()
