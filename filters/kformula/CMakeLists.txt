kde4_header()

add_subdirectory( png ) 
add_subdirectory( latex ) 
add_subdirectory( mathml ) 
add_subdirectory( svg ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = png latex mathml svg
