kde4_header()

add_subdirectory( import )
add_subdirectory( export )
add_subdirectory( export )




########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#if include_wpd_filter
#SUBDIRS = import export
#else
#SUBDIRS = export
#endif
