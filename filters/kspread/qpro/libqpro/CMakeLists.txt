kde4_header()

add_subdirectory( qpro ) 
add_subdirectory( src ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = qpro src
#
#EXTRA_DIST = libqpro.lsm 
