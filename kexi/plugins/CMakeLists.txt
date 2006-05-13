

add_subdirectory( tables )
add_subdirectory( relations )
add_subdirectory( migration )
add_subdirectory( queries )
add_subdirectory( forms )
add_subdirectory( importexport )

if(KOFFICE_ENABLE_SCRIPTING)
    add_subdirectory(scripting)
endif(KOFFICE_ENABLE_SCRIPTING)


message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(REPORTS)")



########### install files ###############






#original Makefile.am contents follow:

#if compile_kexi_reports_plugin
#  REPORTS=reports
#endif
#
#if compile_kross
#  SCRIPTINGDIR=scripting
#endif
#
#SUBDIRS = tables relations migration queries forms $(REPORTS) $(SCRIPTINGDIR) importexport
#
##importwizard relations
