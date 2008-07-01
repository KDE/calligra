add_subdirectory( tables )
add_subdirectory( queries )
add_subdirectory( forms )
add_subdirectory( migration )
add_subdirectory( importexport )
add_subdirectory( relations )

if(SHOULD_BUILD_KSPREAD)
  macro_optional_add_subdirectory( reportspgz )
endif(SHOULD_BUILD_KSPREAD)

#if(DEFINED DKEXI_SCRIPTS_SUPPORT)
    add_subdirectory(scripting)
#endif(DEFINED DKEXI_SCRIPTS_SUPPORT)

#add_subdirectory(macros)

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(REPORTS)")
