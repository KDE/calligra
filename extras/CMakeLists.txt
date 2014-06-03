if(SHOULD_BUILD_CALLIGRA)
  add_subdirectory( calligra )
endif(SHOULD_BUILD_CALLIGRA)

if (SHOULD_BUILD_CONVERTER)
    add_subdirectory( converter )
endif(SHOULD_BUILD_CONVERTER)

if (SHOULD_BUILD_EXTRA_PROPERTIES)
    add_subdirectory( properties )
endif(SHOULD_BUILD_EXTRA_PROPERTIES)

if (SHOULD_BUILD_EXTRA_THUMBNAIL)
    add_subdirectory( thumbnail )
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)

if (SHOULD_BUILD_EXTRA_QUICKPRINT)
    add_subdirectory( quickprint )
endif(SHOULD_BUILD_EXTRA_QUICKPRINT)

if(SHOULD_BUILD_EXTRA_FILEMANAGER_TEMPLATES)
    add_subdirectory( filemanagertemplates )
endif(SHOULD_BUILD_EXTRA_FILEMANAGER_TEMPLATES)

if(SHOULD_BUILD_OKULARODPGENERATOR)
    add_subdirectory( okularodpgenerator )
endif(SHOULD_BUILD_OKULARODPGENERATOR)
