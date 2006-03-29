kde4_header()

include_directories(${KFORMULA_INCLUDES})

add_subdirectory( png )
add_subdirectory( latex )
add_subdirectory( mathml )
add_subdirectory( svg )




########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = png latex mathml svg
