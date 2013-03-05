macro_optional_add_subdirectory( converter )
macro_optional_add_subdirectory( properties )
macro_optional_add_subdirectory( thumbnail )
macro_optional_add_subdirectory( quickprint )

if(SHOULD_BUILD_CALLIGRA)
  macro_optional_add_subdirectory(calligra)
endif(SHOULD_BUILD_CALLIGRA)

if (NOT CREATIVEONLY)

macro_optional_find_package(Okular)
macro_log_feature(OKULAR_FOUND "Okular" "Okular ODP Plugin" "http://okular.kde.org/" FALSE "" "Required to build the Okular OpenDocument Presenter plugin")

if(OKULAR_FOUND AND NOT CREATIVEONLY)
    add_subdirectory( okularodpgenerator )
endif(OKULAR_FOUND AND NOT CREATIVEONLY)

endif (NOT CREATIVEONLY)
