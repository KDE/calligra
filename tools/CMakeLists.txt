kde4_header()

add_subdirectory( converter )
add_subdirectory( thumbnail )
add_subdirectory( thesaurus )
add_subdirectory( kthesaurus )
add_subdirectory( kfile-plugins )
add_subdirectory( quickprint )




########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = converter thumbnail thesaurus kthesaurus kfile-plugins quickprint
## Not ported to kspell2: spell. But do we really want a separate tool for this?
