kde4_header()

add_subdirectory( libimageexport ) 
add_subdirectory( bmp ) 
add_subdirectory( jpeg ) 
add_subdirectory( mng ) 
add_subdirectory( png ) 
add_subdirectory( svg ) 
add_subdirectory( xbm ) 
add_subdirectory( xpm ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = libimageexport bmp jpeg mng png svg xbm xpm
