set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  ${KDE4_ENABLE_EXCEPTIONS}")
set(libavoid_LIB_SRCS
	connectionpin.cpp
	connector.cpp
	connend.cpp
	geometry.cpp
	geomtypes.cpp
	graph.cpp
	hyperedge.cpp
	hyperedgetree.cpp
	junction.cpp
	makepath.cpp
	mtst.cpp
	obstacle.cpp
	orthogonal.cpp
	router.cpp
	shape.cpp
	timer.cpp
	vertices.cpp
	viscluster.cpp
	visibility.cpp
	vpsc.cpp
)

kde4_add_library(libavoid SHARED ${libavoid_LIB_SRCS})

set_target_properties(libavoid PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS libavoid ${INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
	libavoid.h
	DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel
)
