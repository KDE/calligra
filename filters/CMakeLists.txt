include(CheckIncludeFiles)

find_package(GraphicsMagick)
macro_optional_find_package(LibXml2)
macro_optional_find_package(LibXslt)
# for xsltfilter:
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
# for xpdf:
check_include_files(ndir.h HAVE_NDIR_H)
check_include_files(sys/dir.h HAVE_SYS_DIR_H)
check_include_files(sys/ndir.h HAVE_SYS_NDIR_H)
check_include_files(dirent.h HAVE_DIRENT_H)
# others:
check_include_files(ieeefp.h HAVE_IEEEFP_H)
check_include_files(dirent.h HAVE_DIRENT_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(strings.h HAVE_STRINGS_H)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/config-filters.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-filters.h )

set(libdialogfilter_SRCS 
	${CMAKE_SOURCE_DIR}/filters/libdialogfilter/exportsizedia.cpp
)

set(liboofilter_SRCS 
	${CMAKE_SOURCE_DIR}/filters/liboofilter/ooutils.cc 
	${CMAKE_SOURCE_DIR}/filters/liboofilter/liststylestack.cc
	)

set(KWMF_INCLUDES ${CMAKE_SOURCE_DIR}/filters/libkowmf)

add_subdirectory( generic_wrapper )
add_subdirectory( libkowmf )
macro_optional_add_subdirectory(kchart)
macro_optional_add_subdirectory(kword)
if(SHOULD_BUILD_KPRESENTER)
    macro_optional_add_subdirectory(kpresenter)
endif(SHOULD_BUILD_KPRESENTER)
#macro_optional_add_subdirectory(kformula)

if(SHOULD_BUILD_KSPREAD)
    macro_optional_add_subdirectory(kspread)
endif(SHOULD_BUILD_KSPREAD)

if(SHOULD_BUILD_KARBON)
  macro_optional_add_subdirectory(karbon)
endif(SHOULD_BUILD_KARBON)

if (SHOULD_BUILD_KRITA)
  if(NOT MSVC) # TODO reenable
    macro_optional_add_subdirectory(krita)
  endif(NOT MSVC)
endif(SHOULD_BUILD_KRITA)

#macro_optional_add_subdirectory(kivio)
if(LIBXML2_FOUND AND LIBXSLT_FOUND)
	add_subdirectory(xsltfilter)
endif(LIBXML2_FOUND AND LIBXSLT_FOUND)	

message (STATUS "Re-add kivio filters, karbon filter, kpresenter filters")
MESSAGE( STATUS "Readd or not Kformula filter: Status ?")

########### install files ###############
