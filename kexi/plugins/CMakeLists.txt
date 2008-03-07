add_subdirectory( tables )
add_subdirectory( queries )
add_subdirectory( forms )
add_subdirectory( migration )
add_subdirectory( importexport )
add_subdirectory( relations )
#error:
# dashbot	In file included from koffice-783359/kexi/plugins/reportspgz/backend/common/krpos.cpp:20:
#	dashbot	koffice-783359/kexi/plugins/reportspgz/backend/common/krpos.h:22:22: error: KoGlobal.h: No such file or directory
#
#macro_optional_add_subdirectory( reportspgz )

#if(DEFINED DKEXI_SCRIPTS_SUPPORT)
    add_subdirectory(scripting)
#endif(DEFINED DKEXI_SCRIPTS_SUPPORT)

#add_subdirectory(macros)

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(REPORTS)")
