macro_optional_add_subdirectory( converter )
macro_optional_add_subdirectory( thumbnail )
macro_optional_add_subdirectory( quickprint )

if (NOT CREATIVEONLY)

macro_optional_add_subdirectory( slidecompare )
if(SHOULD_BUILD_CSTESTER)
    macro_optional_add_subdirectory( cstester )
endif(SHOULD_BUILD_CSTESTER)

if(SHOULD_BUILD_CALLIGRA)
  macro_optional_add_subdirectory(calligra)
endif(SHOULD_BUILD_CALLIGRA)

macro_optional_find_package(Okular)
macro_log_feature(OKULAR_FOUND "Okular" "Okular ODP Plugin" "http://okular.kde.org/" FALSE "" "Required to build the Okular OpenDocument Presenter plugin")

if(OKULAR_FOUND)
    add_subdirectory( okularodpgenerator )
endif(OKULAR_FOUND)

add_subdirectory(scripts)

endif (NOT CREATIVEONLY)
