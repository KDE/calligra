if(SHOULD_BUILD_APP_CALLIGRA)
  add_subdirectory( calligra )
endif(SHOULD_BUILD_APP_CALLIGRA)

if (SHOULD_BUILD_APP_CONVERTER)
    add_subdirectory( converter )
endif(SHOULD_BUILD_APP_CONVERTER)

if (SHOULD_BUILD_FILEMANAGER_PROPERTIES)
    add_subdirectory( properties )
endif(SHOULD_BUILD_FILEMANAGER_PROPERTIES)

if (SHOULD_BUILD_FILEMANAGER_THUMBNAIL)
    add_subdirectory( thumbnail )
endif(SHOULD_BUILD_FILEMANAGER_THUMBNAIL)

if (SHOULD_BUILD_FILEMANAGER_QUICKPRINT)
    add_subdirectory( quickprint )
endif(SHOULD_BUILD_FILEMANAGER_QUICKPRINT)

if(SHOULD_BUILD_FILEMANAGER_TEMPLATES)
    add_subdirectory( filemanagertemplates )
endif(SHOULD_BUILD_FILEMANAGER_TEMPLATES)

if(SHOULD_BUILD_OKULAR_GENERATOR_ODP)
    add_subdirectory( okularodpgenerator )
endif(SHOULD_BUILD_OKULAR_GENERATOR_ODP)

if(SHOULD_BUILD_OKULAR_GENERATOR_ODT)
    add_subdirectory( okularodtgenerator )
endif(SHOULD_BUILD_OKULAR_GENERATOR_ODT)
