kde4_header()

add_subdirectory( import ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

##TODO: export
#SUBDIRS = import
