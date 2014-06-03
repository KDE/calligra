if(SHOULD_BUILD_FILTER_APPLIXSPREAD_TO_KSPREAD)
    add_subdirectory( applixspread )
endif(SHOULD_BUILD_FILTER_APPLIXSPREAD_TO_KSPREAD)

add_subdirectory( csv )

if(SHOULD_BUILD_FILTER_DBASE_TO_KSPREAD)
    add_subdirectory( dbase )
endif(SHOULD_BUILD_FILTER_DBASE_TO_KSPREAD)

add_subdirectory( gnumeric )
add_subdirectory( opencalc )

if(SHOULD_BUILD_FILTER_QUATTROPRO_TO_SHEETS)
    add_subdirectory( qpro )
endif(SHOULD_BUILD_FILTER_QUATTROPRO_TO_SHEETS)

add_subdirectory( html )
add_subdirectory( latex )

add_subdirectory( excel )

if(SHOULD_BUILD_FILTER_XLSX_TO_ODS)
    add_subdirectory( xlsx )
endif(SHOULD_BUILD_FILTER_XLSX_TO_ODS)
