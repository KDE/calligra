cmake_minimum_required(VERSION 2.8.0)

#cmake_policy CMP0017 was introduced in version 2.8.4
if(${CMAKE_VERSION} VERSION_GREATER 2.8.3)
    cmake_policy(SET CMP0017 NEW)
endif()

# wv2 versioning
set( WV2_MAJOR_VERSION 0 )
set( WV2_MINOR_VERSION 9 )
set( WV2_MICRO_VERSION 0 )
set( WV2_VERSION ${WV2_MAJOR_VERSION}.${WV2_MINOR_VERSION}.${WV2_MICRO_VERSION} )

# libtool versioning
set( LT_VERSION_CURRENT 9 )
set( LT_VERSION_REVISION 0 )
set( LT_VERSION_AGE 0 )

# For automake. Is this required in CMake? (I don't think so)
set( VERSION ${WV2_VERSION} )
set( PACKAGE wv2 )

option( WITH_ZLIB "Build wv2 with zlib (with compression features)" ON )
if( WITH_ZLIB )
  find_package( ZLIB REQUIRED )
endif()

include_directories( ${LIBMSO_INCLUDE_DIR} ${ICONV_INCLUDE_DIR} ${ZLIB_INCLUDE_DIR} )

#
# Various checks
#

include( TestBigEndian )
test_big_endian( WORDS_BIGENDIAN )

check_include_file( dlfcn.h HAVE_DLFCN_H )
check_include_file( strings.h HAVE_STRINGS_H )
check_include_file( string.h HAVE_STRING_H )
check_include_file( math.h HAVE_MATH_H )
check_include_file( float.h HAVE_FLOAT_H )
check_include_file( ieeefp.h HAVE_IEEEFP_H )
check_include_file( errno.h HAVE_ERRNO_H )
check_include_file( inttypes.h HAVE_INTTYPES_H )
check_include_file( memory.h HAVE_MEMORY_H )
check_include_file( stdlib.h HAVE_STDLIB_H )
check_include_file( unistd.h HAVE_UNISTD_H )
check_include_file( stdint.h HAVE_STDINT_H ) # Not really needed because CHECK_TYPE_SIZE already performs this test
check_include_file( stdint.h HAVE_STDINT_H ) # Not really needed because CHECK_TYPE_SIZE already performs this test
check_include_file( sys/types.h HAVE_SYS_TYPES_H ) # Not really needed because CHECK_TYPE_SIZE already performs this test
check_include_file( sys/stat.h HAVE_SYS_STAT_H )

check_type_size( char SIZEOF_CHAR )
check_type_size( short SIZEOF_SHORT )
check_type_size( long SIZEOF_LONG )
check_type_size( int SIZEOF_INT )
check_type_size( "void *" SIZEOF_VOID_P )

if( NOT MSVC )
  # libm does not exist on MSVC
  set( CMAKE_REQUIRED_LIBRARIES m )
  set( CMAKE_REQUIRED_INCLUDES math.h )
endif()

check_function_exists( isinf HAVE_FUNC_ISINF )
check_function_exists( isnan HAVE_FUNC_ISNAN )
check_function_exists( finite HAVE_FUNC_FINITE )
check_function_exists( _finite HAVE_FUNC__FINITE )

#
# Check zlib is modern enough
#

set( NEON_ZLIB 0 ) # By default, we are not modern enough
set( CMAKE_REQUIRED_LIBRARIES ${ZLIB_LIBRARIES} )
set( CMAKE_REQUIRED_INCLUDES zlib.h )

check_function_exists( inflate ZLIB_HAS_INFLATE )

if( ZLIB_HAS_INFLATE )
  try_run( MODERN_ZLIB_RUN MODERN_ZLIB_COMPILE ${CMAKE_CURRENT_BINARY_DIR}/CMakeTmp ${CMAKE_SOURCE_DIR}/cmake/TestModernZlib.c )
endif()

if( MODERN_ZLIB_RUN GREATER 0 AND WITH_ZLIB )
  message( FATAL_ERROR "Your version of zlib is too old for wv2" )
endif()

#
# Set cflags and ldflags
#

if( ZLIB_FOUND )
  set( _WV2_LDFLAGS ${_WV2_LDFLAGS} ${ZLIB_LIBRARIES} )
  set( _WV2_CFLAGS ${_WV2_CFLAGS} ${ZLIB_INCLUDE_DIR} )
endif()

if( ICONV_FOUND )
  set( _WV2_LDFLAGS ${_WV2_LDFLAGS} ${ICONV_LIBRARIES} )
  set( _WV2_CFLAGS ${_WV2_CFLAGS} ${ICONV_INCLUDE_DIR} )
endif()

#
# Clean and prepare
#
list( REMOVE_DUPLICATES _WV2_LDFLAGS )
list( REMOVE_DUPLICATES _WV2_CFLAGS )

foreach( _lib ${_WV2_LDFLAGS} )
  # Remove path to the library and suffixes. Transformation example: libglib-2.0.so => glib-2.0
  string( REGEX REPLACE "[\\\\ _\\/\\.a-zA-Z0-9\\-]*\\/lib([_\\.a-zA-Z0-9\\-]*)\\.[_a-zA-Z0-9\\-\\.]*" \\1 _lib_no_path ${_lib} )
  set( WV2_LDFLAGS "${WV2_LDFLAGS} ${CMAKE_LINK_LIBRARY_FLAG}${_lib_no_path}" )
endforeach( _lib )

foreach( _inc ${_WV2_CFLAGS} )
  set( WV2_CFLAGS "${WV2_CFLAGS} -I${_inc}" )
endforeach( _inc )

# Generate configuration files
configure_file( config.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/config.h )
configure_file( wv2-config.cmake ${CMAKE_CURRENT_BINARY_DIR}/src/wv2-config @ONLY )

# Source directories
add_subdirectory( src )
