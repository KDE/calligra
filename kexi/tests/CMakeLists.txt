kde4_header()

add_subdirectory( newapi ) 
add_subdirectory( widgets ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#
#SUBDIRS = newapi widgets
#
## unused: parser tableview
## startup
#
