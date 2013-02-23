if(SHOULD_BUILD_CALLIGRA)
  macro_optional_add_subdirectory( calligra )
endif(SHOULD_BUILD_CALLIGRA)

if (SHOULD_BUILD_CONVERTER)
    macro_optional_add_subdirectory( converter )
endif(SHOULD_BUILD_CONVERTER)

if (SHOULD_BUILD_EXTRA_PROPERTIES)
    macro_optional_add_subdirectory( properties )
endif(SHOULD_BUILD_EXTRA_PROPERTIES)

if (SHOULD_BUILD_EXTRA_THUMBNAIL)
    macro_optional_add_subdirectory( thumbnail )
endif(SHOULD_BUILD_EXTRA_THUMBNAIL)

if (SHOULD_BUILD_EXTRA_QUICKPRINT)
    macro_optional_add_subdirectory( quickprint )
endif(SHOULD_BUILD_EXTRA_QUICKPRINT)

if(SHOULD_BUILD_OKULARODPGENERATOR)
    macro_optional_find_package(Okular)
    macro_log_feature(OKULAR_FOUND "Okular" "Okular ODP Plugin" "http://okular.kde.org/" FALSE "" "Required to build the Okular OpenDocument Presenter plugin")

    if(OKULAR_FOUND)
        macro_optional_add_subdirectory( okularodpgenerator )
    endif(OKULAR_FOUND)
endif(SHOULD_BUILD_OKULARODPGENERATOR)
