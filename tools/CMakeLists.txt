macro_optional_add_subdirectory( converter )
macro_optional_add_subdirectory( thumbnail )
macro_optional_add_subdirectory( kthesaurus )
macro_optional_add_subdirectory( quickprint )
macro_optional_add_subdirectory( slidecompare )

macro_optional_find_package(Okular)

if(Okular_FOUND)
    add_subdirectory( okularodpgenerator )
else(Okular_FOUND)
    message(STATUS "Did NOT find Okular libraries, Okular generator will not be build")
endif(Okular_FOUND)

macro_optional_find_package(X11)
macro_optional_find_package(Qt4)
if(X11_FOUND)
if(Q_WS_X11 AND SHOULD_BUILD_KSPREAD)
  macro_optional_add_subdirectory( f-office )
endif(Q_WS_X11 AND SHOULD_BUILD_KSPREAD)
endif(X11_FOUND)

# strigi ships with the odfsaxanalyzer which does already fetch the meta-data of
# odf files. Also the current implementations of the analyzers are deactivated
# anyway. So, what would be needed is to connect somehow with KoStore or hook
# in the koffice-filters another way somehow.
#add_subdirectory( strigi-analyzer )
