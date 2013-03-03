if(SHOULD_BUILD_CSTESTER)
    add_subdirectory( cstester )
endif(SHOULD_BUILD_CSTESTER)

if (SHOULD_BUILD_DEVTOOLS)
    add_subdirectory(slidecompare)
    add_subdirectory(scripts)
endif (SHOULD_BUILD_DEVTOOLS)

