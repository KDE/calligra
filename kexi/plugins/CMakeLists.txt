add_definitions(-DKDE_DEFAULT_DEBUG_AREA=44021)

add_subdirectory( reports )

if (KEXI_MOBILE)


else ()
  add_subdirectory( tables )
  add_subdirectory( queries )
  add_subdirectory( forms )
  add_subdirectory( importexport )
  # KEXI3 TODO add_subdirectory( migration )

  if(SHOULD_BUILD_FEATURE_SCRIPTING AND KEXI_SCRIPTS_SUPPORT)
     add_subdirectory(scripting)
  endif()
endif ()
