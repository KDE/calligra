
find_package(GraphicsMagick)
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
add_subdirectory(kchart)
add_subdirectory(kword)
add_subdirectory(kpresenter)
#add_subdirectory(kformula)

if(SHOULD_BUILD_KSPREAD)
    add_subdirectory(kspread)
endif(SHOULD_BUILD_KSPREAD)

if(SHOULD_BUILD_KARBON)
  #add_subdirectory(karbon)
endif(SHOULD_BUILD_KARBON)

if (SHOULD_BUILD_KRITA)
  add_subdirectory(krita)
endif(SHOULD_BUILD_KRITA)
#add_subdirectory(kivio)
if(LIBXML2_FOUND AND LIBXSLT_FOUND)
	add_subdirectory(xsltfilter)
endif(LIBXML2_FOUND AND LIBXSLT_FOUND)	

message (STATUS "Re-add kivio filters, karbon filter")
MESSAGE( STATUS "Readd or not Kformula filter: Status ?")

########### install files ###############
