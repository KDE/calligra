
add_subdirectory( icalendar )

if(WIN32)
    #disable for now
    #add_subdirectory( kplato )
else(WIN32)
    add_subdirectory( kplato )
endif(WIN32)
