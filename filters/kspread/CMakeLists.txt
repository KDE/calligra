kde4_header()

include_directories(
    ${KOTEXT_INCLUDES}
    ${KOFFICEUI_INCLUDES}
	)

add_subdirectory( applixspread )
add_subdirectory( csv )
add_subdirectory( dbase )
add_subdirectory( gnumeric )
add_subdirectory( latex )
add_subdirectory( opencalc )
add_subdirectory( html )
add_subdirectory( qpro )
add_subdirectory( excel )




########### install files ###############


kde4_footer()



#original Makefile.am contents follow:

#SUBDIRS = applixspread csv dbase gnumeric latex opencalc html qpro excel
