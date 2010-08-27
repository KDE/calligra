include(CheckIncludeFiles)

macro_optional_find_package(LibXml2)
macro_log_feature(LIBXML2_FOUND "LibXML2" "LibXml2 library" "http://xmlsoft.org/" FALSE "" "Required by the xlslt filter")

macro_optional_find_package(LibXslt)
macro_log_feature(LIBXSLT_FOUND "LibXslt" "LibXslt Library" "http://xmlsoft.org/XSLT/" FALSE "" "Required by the xslt filter")

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

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")


set(KWMF_INCLUDES ${CMAKE_SOURCE_DIR}/filters/libkowmf)

add_subdirectory(libmso)

macro_optional_add_subdirectory( generic_wrapper )
macro_optional_add_subdirectory( libkowmf )
macro_optional_add_subdirectory( libmsooxml )

if(SHOULD_BUILD_KCHART)
    macro_optional_add_subdirectory(kchart)
endif(SHOULD_BUILD_KCHART)

if (SHOULD_BUILD_KWORD)
    macro_optional_add_subdirectory(kword)
endif (SHOULD_BUILD_KWORD)

if(SHOULD_BUILD_KPRESENTER)
    macro_optional_add_subdirectory(kpresenter)
endif(SHOULD_BUILD_KPRESENTER)

if(SHOULD_BUILD_KFORMULA)
    macro_optional_add_subdirectory(kformula)
endif(SHOULD_BUILD_KFORMULA)

if(SHOULD_BUILD_KSPREAD)
    macro_optional_add_subdirectory(kspread)
endif(SHOULD_BUILD_KSPREAD)

if(SHOULD_BUILD_KARBON)
  macro_optional_add_subdirectory(karbon)
endif(SHOULD_BUILD_KARBON)

if(SHOULD_BUILD_KIVIO)
    #macro_optional_add_subdirectory(kivio)
endif(SHOULD_BUILD_KIVIO)

if(SHOULD_BUILD_KPLATO)
    macro_optional_add_subdirectory(kplato)
endif(SHOULD_BUILD_KPLATO)

if(NOT TINY AND LIBXML2_FOUND AND LIBXSLT_FOUND AND NOT WIN32 AND NOT QT_MAC_USE_COCOA)
	add_subdirectory(xsltfilter)
endif(NOT TINY AND LIBXML2_FOUND AND LIBXSLT_FOUND AND NOT WIN32 AND NOT QT_MAC_USE_COCOA)

########### install files ###############
