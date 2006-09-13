
find_package(ImageMagick)
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
add_subdirectory(kpresenter)
#add_subdirectory(kformula)
#add_subdirectory(karbon)
add_subdirectory(krita)
#add_subdirectory(kivio)
if(LIBXML2_FOUND AND LIBXSLT_FOUND)
	add_subdirectory(xsltfilter)
endif(LIBXML2_FOUND AND LIBXSLT_FOUND)	

message (STATUS "Re-add kivio filters, karbon filter")


########### install files ###############
