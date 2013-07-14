# check whether we can compile wv2

macro_optional_find_package(Iconv)
macro_log_feature(ICONV_FOUND "Iconv" "Iconv" "http://www.gnu.org/software/libiconv/" FALSE "" "Required by the msword .doc filter")
IF( ICONV_FOUND )
    SET( HAVE_ICONV_H 1 )
    SET( ICONV_REQUIRES_CONST ${ICONV_SECOND_ARGUMENT_IS_CONST} )
ENDIF( ICONV_FOUND )

INCLUDE( CheckIncludeFile )

CHECK_INCLUDE_FILE( sys/iconv.h HAVE_SYS_ICONV_H )

# Add "COMPILE_DEFINITIONS definitions" to TRY_RUN only if we have compile definitions

# Make sure ICONV_COMPILE_DEFINITIONS will never be empty (in case we define neither HAVE_ICONV_H nor HAVE_SYS_ICONV_H),
# otherwise TRY_RUN will fail due to COMPILE_DEFINITIONS being followed by nothing

SET( ICONV_COMPILE_DEFINITIONS "-DBLAH" )

IF( HAVE_ICONV_H )
  SET( ICONV_COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} "-DHAVE_ICONV_H" )
ENDIF( HAVE_ICONV_H )

IF( HAVE_SYS_ICONV_H )
  SET( ICONV_COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} "-DHAVE_SYS_ICONV_H" )
ENDIF( HAVE_SYS_ICONV_H )

TRY_RUN( MODERN_ICONV_RUN MODERN_ICONV_COMPILE
         ${CMAKE_CURRENT_BINARY_DIR}/CMakeTmp
         ${CMAKE_SOURCE_DIR}/cmake/TestModernIconv.c
         CMAKE_FLAGS "-DLINK_LIBRARIES:STRING=${ICONV_LIBRARIES}"
                     "-DINCLUDE_DIRECTORIES:STRING=${ICONV_INCLUDE_DIR}"
         COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} )

IF( MODERN_ICONV_RUN GREATER 0 OR NOT MODERN_ICONV_COMPILE )
  MESSAGE( STATUS "wv2 depends on a modern iconv installation, supporting UNICODELITTLE and" )
  MESSAGE( STATUS "UNICODEBIG. The detected iconv version doesn't support these conversions." )
  MESSAGE( STATUS "" )
  MESSAGE( STATUS "Please get a new libiconv from http://www.gnu.org/software/libiconv/" )
  MESSAGE( STATUS "You might want to install the library to some alternative directory, in" )
  MESSAGE( STATUS "order not to overwrite your current installation. Please use the options" )
  MESSAGE( STATUS "-DICONV_INCLUDE_DIR=DIR and -DICONV_LIBRARIES=DIR to specify the location." )
  MESSAGE( STATUS "" )
  set(ICONV_FOUND FALSE)
ENDIF( MODERN_ICONV_RUN GREATER 0 OR NOT MODERN_ICONV_COMPILE )


include_directories(${KOMAIN_INCLUDES} )

if(SHOULD_BUILD_FILTER_APPLIXWORD_TO_ODT)
    add_subdirectory( applixword )
endif(SHOULD_BUILD_FILTER_APPLIXWORD_TO_ODT)

add_subdirectory( wordperfect )

add_subdirectory( works )

add_subdirectory( ascii )

add_subdirectory( wiki )

add_subdirectory( rtf )

add_subdirectory( epub )

if(SHOULD_BUILD_FILTER_ODT_TO_MOBI)
    add_subdirectory( mobi )
endif(SHOULD_BUILD_FILTER_ODT_TO_MOBI)

if(SHOULD_BUILD_FILTER_DOC_TO_ODT)
    add_subdirectory(msword-odf)
endif(SHOULD_BUILD_FILTER_DOC_TO_ODT)

if(SHOULD_BUILD_FILTER_DOCX_TO_ODT)
    add_subdirectory( docx )
endif(SHOULD_BUILD_FILTER_DOCX_TO_ODT)
