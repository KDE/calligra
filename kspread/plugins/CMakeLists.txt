add_subdirectory( insertcalendar )
add_subdirectory( solver )

if(KOFFICE_ENABLE_SCRIPTING)
    add_subdirectory(scripting)
endif(KOFFICE_ENABLE_SCRIPTING)

