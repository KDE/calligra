if(SHOULD_BUILD_APP_CALLIGRA)
    #add_subdirectory( calligra )
endif()

if (SHOULD_BUILD_APP_CONVERTER)
    add_subdirectory( converter )
endif()

if (SHOULD_BUILD_FILEMANAGER_PROPERTIES)
    add_subdirectory( properties )
endif()

if (SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    add_subdirectory( thumbnail )
endif()

if (SHOULD_BUILD_FILEMANAGER_QUICKPRINT)
    add_subdirectory( quickprint )
endif()

if(SHOULD_BUILD_FILEMANAGER_TEMPLATES)
    add_subdirectory( filemanagertemplates )
endif()

if(SHOULD_BUILD_OKULAR_GENERATOR_ODP)
    add_subdirectory( okularodpgenerator )
endif()

if(SHOULD_BUILD_OKULAR_GENERATOR_ODT)
    add_subdirectory( okularodtgenerator )
endif()
