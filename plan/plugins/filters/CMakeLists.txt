
add_subdirectory( icalendar )

if(WIN32)
    #disable for now
    #add_subdirectory( kplato )
else()
    add_subdirectory( kplato )
endif()
