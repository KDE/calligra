if (SHOULD_BUILD_KDGANTT)
    macro_optional_add_subdirectory(kdgantt)
endif (SHOULD_BUILD_KDGANTT)

if (SHOULD_BUILD_KDCHART)
    add_subdirectory(kdchart)
endif (SHOULD_BUILD_KDCHART)
