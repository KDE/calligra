

macro_optional_find_package(LibXml2)
macro_optional_find_package(LibXslt)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-filters.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-filters.h )

set(libdialogfilter_SRCS 
	${CMAKE_SOURCE_DIR}/filters/libdialogfilter/exportsizedia.cpp
)

set(liboofilter_SRCS 
	${CMAKE_SOURCE_DIR}/filters/liboofilter/ooutils.cc 
	${CMAKE_SOURCE_DIR}/filters/liboofilter/liststylestack.cc
	)

add_subdirectory( generic_wrapper )
add_subdirectory( libdialogfilter )
add_subdirectory( liboofilter )
add_subdirectory(kspread)
add_subdirectory(kchart)
add_subdirectory(kword)
#add_subdirectory(kpresenter)
add_subdirectory(kformula)
add_subdirectory(karbon)
add_subdirectory(krita)
#add_subdirectory(kivio)
if(LIBXML2_FOUND AND LIBXSLT_FOUND)
	add_subdirectory(xsltfilter)
endif(LIBXML2_FOUND AND LIBXSLT_FOUND)	

message (STATUS "Re-add karbon kivio kpresenter filters")


########### install files ###############







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
