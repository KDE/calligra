
add_subdirectory( tables )
add_subdirectory( queries )
add_subdirectory( forms )
add_subdirectory( migration )
add_subdirectory( importexport )
add_subdirectory( relations )

# disabled for now till ported to Kross2
#add_subdirectory(scripting)
#add_subdirectory(macros)

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
