
if(KdepimLibs_FOUND)
    add_subdirectory( icalendar )
endif(KdepimLibs_FOUND)

if(WIN32)
    #disable for now
    #add_subdirectory( kplato )
else(WIN32)
    add_subdirectory( kplato )
endif(WIN32)
