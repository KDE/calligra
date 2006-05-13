

include_directories( ${KOFFICECORE_INCLUDES})
add_subdirectory( converter )
add_subdirectory( thumbnail )
add_subdirectory( thesaurus )
add_subdirectory( kthesaurus )
add_subdirectory( kfile-plugins )
add_subdirectory( quickprint )




########### install files ###############






#original Makefile.am contents follow:

#SUBDIRS = converter thumbnail thesaurus kthesaurus kfile-plugins quickprint
