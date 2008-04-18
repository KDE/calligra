add_subdirectory( tables )
add_subdirectory( queries )
add_subdirectory( forms )
add_subdirectory( migration )
add_subdirectory( importexport )
add_subdirectory( relations )
#piggz, please fix the compilation:
#CMake Error: Cannot find source file "/home/boud/kde/src/koffice/kexi/plugins/reportspgz/backend/wrtembed/reportentitychart.cpp"
#macro_optional_add_subdirectory( reportspgz )

#if(DEFINED DKEXI_SCRIPTS_SUPPORT)
    add_subdirectory(scripting)
#endif(DEFINED DKEXI_SCRIPTS_SUPPORT)

#add_subdirectory(macros)

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(REPORTS)")
