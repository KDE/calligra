
if(CMAKE_CROSSCOMPILING)
  set(IMPORT_RECORDSXML2CPPEXECUTABLE "IMPORTFILE-NOTFOUND" CACHE FILEPATH "Point it to the export file from a native build")
  include(${IMPORT_RECORDSXML2CPPEXECUTABLE})
endif(CMAKE_CROSSCOMPILING)

add_subdirectory( sidewinder )
add_subdirectory( import )
add_subdirectory( export )

include_directories( ${CMAKE_SOURCE_DIR}/sheets ${KDE4_INCLUDES}  )

