kde4_header()

add_subdirectory( koffice ) 
add_subdirectory( ooo ) 
add_subdirectory( gnumeric ) 
add_subdirectory( abiword ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS=koffice ooo gnumeric abiword
