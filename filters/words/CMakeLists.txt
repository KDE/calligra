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

IF( NOT TINY )
    macro_optional_find_package(WPD)
    macro_log_feature(WPD_FOUND "WPD" "WordPerfect Document Library" "http://libwpd.sourceforge.net/" FALSE "" "Required by the Words WPD import filter")
    macro_optional_find_package(WPG)
    macro_log_feature(LIBWPG_FOUND "WPG" "WordPerfect Graphics Library" "http://libwpg.sourceforge.net/" FALSE "" "Required by the Karbon WPG import filter")
    macro_optional_find_package(LibWPS)
    macro_log_feature(LIBWPS_FOUND "LibWPS" "Microsoft Works Document Library" "http://libwps.sourceforge.net/" FALSE "" "Required by the Words WPS import filter")

    add_subdirectory( applixword )
    add_subdirectory( html-odf )
    if(WPD_FOUND AND WPG_FOUND)
        add_subdirectory( wordperfect )
    endif(WPD_FOUND AND WPG_FOUND)
    if(WPD_FOUND AND LIBWPS_FOUND)
        add_subdirectory( works )
    endif(WPD_FOUND AND LIBWPS_FOUND)
ENDIF( NOT TINY )

add_subdirectory( ascii )
add_subdirectory( rtf )

add_subdirectory( epub )

add_subdirectory(msword-odf)

add_subdirectory( docx )
