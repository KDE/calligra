if(SHOULD_BUILD_APP_CSTESTER)
    add_subdirectory( cstester )
endif()

if (SHOULD_BUILD_APP_SLIDECOMPARE)
    add_subdirectory(slidecompare)
endif()

if (SHOULD_BUILD_APP_DEVTOOLS)
    add_subdirectory(scripts)
endif ()

if (SHOULD_BUILD_BUILDTOOL_RNG2CPP)
    add_subdirectory(rng2cpp)
endif ()
