kde4_header()

add_subdirectory( kexiapp ) 

include_directories( ${KDE4_INCLUDE_DIR} ${QT_INCLUDES}  )


########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#include $(top_srcdir)/kexi/Makefile.global
#SUBDIRS = kexiapp
