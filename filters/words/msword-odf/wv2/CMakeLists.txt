cmake_minimum_required(VERSION 3.2)

add_definitions(-DUNICODE -D_UNICODE)

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

include_directories(${LIBMSO_INCLUDE_DIR})

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

check_symbol_exists( isinf math.h HAVE_FUNC_ISINF )
check_symbol_exists( isnan math.h HAVE_FUNC_ISNAN )
if(MINGW)
add_definitions( -DHAVE_FUNC_ISFINITE )
endif()
check_symbol_exists( finite math.h HAVE_FUNC_FINITE )
check_symbol_exists( _finite math.h HAVE_FUNC__FINITE )
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
