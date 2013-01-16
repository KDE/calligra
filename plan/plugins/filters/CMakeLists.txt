
if(KdepimLibs_FOUND AND QT3SUPPORT)
    macro_optional_add_subdirectory( icalendar )
endif(KdepimLibs_FOUND AND QT3SUPPORT)

if(WIN32)
    #disable for now
    #add_subdirectory( kplato )
else(WIN32)
    add_subdirectory( kplato )
endif(WIN32)
