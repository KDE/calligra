if (NOT CREATIVEONLY)

macro_optional_add_subdirectory( slidecompare )
if(SHOULD_BUILD_CSTESTER)
    macro_optional_add_subdirectory( cstester )
endif(SHOULD_BUILD_CSTESTER)

add_subdirectory(scripts)

endif (NOT CREATIVEONLY)
