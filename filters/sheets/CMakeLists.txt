# TEMPORARY: for  Qt5/KF5 build porting phase deprecation warnings are only annoying noise
if (CMAKE_COMPILER_IS_GNUCXX OR CMAKE_COMPILER_IS_GNUC)
    add_definitions(-Wno-deprecated -Wno-deprecated-declarations)
endif ()

if(SHOULD_BUILD_FILTER_APPLIXSPREAD_TO_KSPREAD)
    add_subdirectory( applixspread )
endif()

add_subdirectory( csv )

if(SHOULD_BUILD_FILTER_DBASE_TO_KSPREAD)
    add_subdirectory( dbase )
endif()

add_subdirectory( gnumeric )
add_subdirectory( opencalc )

if(SHOULD_BUILD_FILTER_QUATTROPRO_TO_SHEETS)
    add_subdirectory( qpro )
endif()

add_subdirectory( html )
add_subdirectory( latex )

add_subdirectory( excel )

if(SHOULD_BUILD_FILTER_XLSX_TO_ODS)
    add_subdirectory( xlsx )
endif()
