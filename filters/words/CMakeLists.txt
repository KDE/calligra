# check whether we can compile wv2

macro_optional_find_package(Iconv)
set_package_properties(Iconv PROPERTIES
    URL "http://www.gnu.org/software/libiconv/"
    PURPOSE "Required by the msword .doc filter"
    TYPE OPTIONAL
)

if( ICONV_FOUND )
    set( HAVE_ICONV_H 1 )
    set( ICONV_REQUIRES_CONST ${ICONV_SECOND_ARGUMENT_IS_CONST} )
endif()

check_include_file( sys/iconv.h HAVE_SYS_ICONV_H )

# Add "COMPILE_DEFINITIONS definitions" to TRY_RUN only if we have compile definitions

# Make sure ICONV_COMPILE_DEFINITIONS will never be empty (in case we define neither HAVE_ICONV_H nor HAVE_SYS_ICONV_H),
# otherwise TRY_RUN will fail due to COMPILE_DEFINITIONS being followed by nothing

set( ICONV_COMPILE_DEFINITIONS "-DBLAH" )

if( HAVE_ICONV_H )
  set( ICONV_COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} "-DHAVE_ICONV_H" )
endif()

if( HAVE_SYS_ICONV_H )
  set( ICONV_COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} "-DHAVE_SYS_ICONV_H" )
endif()

try_run( MODERN_ICONV_RUN MODERN_ICONV_COMPILE
         ${CMAKE_CURRENT_BINARY_DIR}/CMakeTmp
         ${CMAKE_SOURCE_DIR}/cmake/TestModernIconv.c
         CMAKE_FLAGS "-DLINK_LIBRARIES:STRING=${ICONV_LIBRARIES}"
                     "-DINCLUDE_DIRECTORIES:STRING=${ICONV_INCLUDE_DIR}"
         COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} )

if( MODERN_ICONV_RUN GREATER 0 OR NOT MODERN_ICONV_COMPILE )
  message( STATUS "wv2 depends on a modern iconv installation, supporting UNICODELITTLE and" )
  message( STATUS "UNICODEBIG. The detected iconv version doesn't support these conversions." )
  message( STATUS "" )
  message( STATUS "Please get a new libiconv from http://www.gnu.org/software/libiconv/" )
  message( STATUS "You might want to install the library to some alternative directory, in" )
  message( STATUS "order not to overwrite your current installation. Please use the options" )
  message( STATUS "-DICONV_INCLUDE_DIR=DIR and -DICONV_LIBRARIES=DIR to specify the location." )
  message( STATUS "" )
  set(ICONV_FOUND FALSE)
endif()


include_directories(${KOMAIN_INCLUDES} )

if(SHOULD_BUILD_FILTER_APPLIXWORD_TO_ODT)
    add_subdirectory( applixword )
endif()

add_subdirectory( wordperfect )

add_subdirectory( works )

add_subdirectory( ascii )

add_subdirectory( wiki )

add_subdirectory( rtf )

add_subdirectory( epub )

if(SHOULD_BUILD_FILTER_ODT_TO_MOBI)
    add_subdirectory( mobi )
endif()

if(SHOULD_BUILD_FILTER_DOC_TO_ODT)
	if(NOT WIN32 AND NOT BSD) # FIXME
     add_subdirectory(msword-odf)
  endif()
endif()

if(SHOULD_BUILD_FILTER_DOCX_TO_ODT)
    add_subdirectory( docx )
endif()
