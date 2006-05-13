

include_directories(${KOFFICEUI_INCLUDES} ${KFORMULA_INCLUDES})

add_subdirectory( png )
add_subdirectory( latex )
add_subdirectory( mathml )
add_subdirectory( svg )




########### install files ###############






#original Makefile.am contents follow:

#SUBDIRS = png latex mathml svg
