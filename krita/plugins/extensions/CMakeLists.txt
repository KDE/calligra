add_subdirectory( bigbrother )
add_subdirectory( imagesplit )
add_subdirectory( colorrange )
add_subdirectory( colorspaceconversion )
add_subdirectory( compose )
add_subdirectory( dropshadow )
add_subdirectory( histogram )
add_subdirectory( imagesize )
add_subdirectory( metadataeditor )
add_subdirectory( modify_selection )
add_subdirectory( rotateimage )
add_subdirectory( separate_channels )
add_subdirectory( shearimage )
add_subdirectory( dockers )

find_package(OpenShiva)
find_package(QtShiva )

if(HAVE_OPENSHIVA AND HAVE_QTSHIVA)
 add_subdirectory(shiva)
endif(HAVE_OPENSHIVA AND HAVE_QTSHIVA)

macro_log_feature(HAVE_OPENSHIVA "OpenShiva" "OpenShiva interpreter for the Shiva Kernel Language (part of OpenGTL)" "http://www.opengtl.org" FALSE "" "Required for Shiva based Generators and Filters")
macro_log_feature(HAVE_QTSHIVA "QtShiva" "Qt bindings for the OpenShiva interpreter (part of libQtGTL)" "http://www.opengtl.org" FALSE "" "Required for Shiva based Generators and Filters")

find_package(OpenRijn)
find_package(QtRijn )

if(HAVE_OPENRIJN AND HAVE_QTRIJN)
 add_subdirectory(rijn)
endif(HAVE_OPENRIJN AND HAVE_QTRIJN)

macro_log_feature(HAVE_OPENRIJN "OpenRijn" "OpenRijn interpreter for the Rijn Skectch Language (part of OpenGTL)" "http://www.opengtl.org" FALSE "" "Required for Rijn based sketches")
macro_log_feature(HAVE_QTRIJN "QtRijn" "Qt bindings for the OpenRijn interpreter (part of libQtGTL)" "http://www.opengtl.org" FALSE "" "Required for Rijn sketches")

