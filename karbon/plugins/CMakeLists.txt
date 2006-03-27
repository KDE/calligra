kde4_header()

add_subdirectory( flattenpath ) 
add_subdirectory( roundcorners ) 
add_subdirectory( insertknots ) 
add_subdirectory( whirlpinch ) 
add_subdirectory( shadoweffect ) 
add_subdirectory( zoomtool ) 
add_subdirectory( imagetool ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = flattenpath roundcorners insertknots whirlpinch shadoweffect zoomtool imagetool
