macro_optional_find_package(DCMTK)
macro_log_feature(DCMTK_FOUND "DCMTK" "DICOM document processing toolkit" "http://dicom.offis.de/dcmtk.php.en" FALSE "" "Required for processing DICOM structured reports")

# check whether we can compile wv2

macro_optional_find_package(GObject)
macro_log_feature(GOBJECT_FOUND "GObject" "GObject" "http://www.gtk.org/" FALSE "" "Required by the msword .doc filter")

macro_optional_find_package(GLIB2)
macro_log_feature(GLIB2_FOUND "Glib2" "Glib2" "http://www.gtk.org/" FALSE "" "Required by the msword .doc filter")

macro_optional_find_package(LIBGSF)
macro_log_feature(LIBGSF_FOUND "libgsf" "LibGSF" "ftp://ftp.gnome.org/pub/GNOME/sources/libgsf" FALSE "" "Required by the msword .doc filter")

macro_optional_find_package(Iconv)
macro_log_feature(ICONV_FOUND "Iconv" "Iconv" "http://www.gnu.org/software/libiconv/" FALSE "" "Required by the msword .doc filter")
IF( ICONV_FOUND )
    SET( HAVE_ICONV_H 1 )
    SET( ICONV_REQUIRES_CONST ${ICONV_SECOND_ARGUMENT_IS_CONST} )
ENDIF( ICONV_FOUND )

#
# Iconv checks for wv2
# 

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

TRY_RUN( MODERN_ICONV_RUN MODERN_ICONV_COMPILE ${CMAKE_CURRENT_BINARY_DIR}/CMakeTmp ${CMAKE_SOURCE_DIR}/cmake/TestModernIconv.c COMPILE_DEFINITIONS ${ICONV_COMPILE_DEFINITIONS} )

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


include_directories(${CMAKE_SOURCE_DIR}/filters/kword/libexport ${KOMAIN_INCLUDES} )

add_subdirectory( libexport )

if (NOT TINY)

    macro_optional_find_package(WPD)
    macro_log_feature(WPD_FOUND "WPD" "WordPerfect Document Library" "http://libwpd.sourceforge.net/" FALSE "" "Required by the KWord WPD import filter")
    
    add_subdirectory( abiword )
    add_subdirectory( amipro )
    add_subdirectory( applixword )
    add_subdirectory( docbook )
    #add_subdirectory( html-odf )

    #html FIXME: export dialog is Qt 3 version ui file
    if(NOT WIN32 AND NOT QT_MAC_USE_COCOA)
        add_subdirectory( html )
#        if (TARGET kowmf)
#            add_subdirectory( mswrite )
#        endif (TARGET kowmf)
        add_subdirectory( kword1.3 )
        add_subdirectory( hancomword )
        message(STATUS "When latex is reactivated (if?) also port it to GraphicsMagick, to make life easy for Isaac.")
        #add_subdirectory( latex )
    endif(NOT WIN32 AND NOT QT_MAC_USE_COCOA)

    add_subdirectory( oowriter )
    add_subdirectory( palmdoc )
    add_subdirectory( wml )

    if(DCMTK_FOUND)
    	add_subdirectory( dcm )
    endif(DCMTK_FOUND)

    if(WPD_FOUND)
    	add_subdirectory( wordperfect )
    endif(WPD_FOUND)
    
endif (NOT TINY)

add_subdirectory( ascii )
add_subdirectory( rtf )

if(GLIB2_FOUND AND GOBJECT_FOUND AND ICONV_FOUND AND LIBGSF_FOUND)
	add_subdirectory(msword-odf)
endif(GLIB2_FOUND AND GOBJECT_FOUND AND ICONV_FOUND AND LIBGSF_FOUND)

add_subdirectory( docx )
