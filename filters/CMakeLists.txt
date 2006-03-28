kde4_header()

add_subdirectory( generic_wrapper )
add_subdirectory( libdialogfilter )
add_subdirectory( liboofilter )

message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KSPREADDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KCHARTDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KWORDDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KPRESENTERDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(XSLTFILTERDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KFORMULADIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KARBONDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KUGARDIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KRITADIR)")
message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}: skipped subdir $(KIVIODIR)")



########### install files ###############


kde4_create_doxygen_docs(  )

kde4_footer()



#original Makefile.am contents follow:

#
#if compile_filter_KARBON
#KARBONDIR = karbon
#endif
#
#if include_XSLTFILTERS
#XSLTFILTERDIR = xsltfilter
#endif
#
#if compile_filter_KWORD
#KWORDDIR = kword
#endif
#
#if compile_filter_KPRESENTER
#KPRESENTERDIR = kpresenter
#endif
#
#if compile_filter_KSPREAD
#KSPREADDIR = kspread
#endif
#
#if compile_filter_KCHART
#KCHARTDIR = kchart
#endif
#
#if compile_filter_KFORMULA
#KFORMULADIR = kformula
#endif
#
#if compile_filter_KUGAR
#KUGARDIR = kugar
#endif
#
#if compile_filter_KRITA
#KRITADIR = krita
#endif
#
#if compile_filter_KIVIO
#KIVIODIR = kivio
#endif
#
#SUBDIRS = generic_wrapper libdialogfilter liboofilter $(KSPREADDIR) $(KCHARTDIR) $(KWORDDIR) $(KPRESENTERDIR)  $(XSLTFILTERDIR) $(KFORMULADIR) $(KARBONDIR) $(KUGARDIR) $(KRITADIR) $(KIVIODIR)
#
#messages:
#	$(EXTRACTRC) `find . -name \*.ui` > rc.cpp
#	$(XGETTEXT) `find . -name \*.cpp -o -name \*.cc` -o $(podir)/kofficefilters.pot
#
#include $(top_srcdir)/admin/Doxyfile.am
