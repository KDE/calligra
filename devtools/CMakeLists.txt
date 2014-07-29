if(SHOULD_BUILD_APP_CSTESTER)
    add_subdirectory( cstester )
endif(SHOULD_BUILD_APP_CSTESTER)

if (SHOULD_BUILD_APP_DEVTOOLS)
    add_subdirectory(slidecompare)
    add_subdirectory(scripts)
endif (SHOULD_BUILD_APP_DEVTOOLS)

if (SHOULD_BUILD_BUILDTOOL_RNG2CPP)
    add_subdirectory(rng2cpp)
endif (SHOULD_BUILD_BUILDTOOL_RNG2CPP)
