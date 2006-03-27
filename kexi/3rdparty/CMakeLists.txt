kde4_header()

add_subdirectory( kexisql3 ) 
add_subdirectory( kexisql ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#
#
#SUBDIRS = kexisql3 kexisql
#
##unused on !win32: kolibs 
##unused: uuid
