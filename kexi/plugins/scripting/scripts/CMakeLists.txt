kde4_header()

add_subdirectory( exportxhtml ) 
add_subdirectory( importxhtml ) 
add_subdirectory( projectdocumentor ) 
add_subdirectory( copycenter ) 
add_subdirectory( python ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = exportxhtml importxhtml projectdocumentor copycenter python
