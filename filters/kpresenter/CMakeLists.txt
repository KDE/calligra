kde4_header()

add_subdirectory( libimageexport )
add_subdirectory( png )
add_subdirectory( bmp )
add_subdirectory( xbm )
add_subdirectory( xpm )
add_subdirectory( mng )
add_subdirectory( svg )
add_subdirectory( jpeg )
add_subdirectory( kword )
add_subdirectory( ooimpress )
add_subdirectory( powerpoint )




########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = libimageexport png bmp xbm xpm mng svg jpeg kword ooimpress powerpoint
