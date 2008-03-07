include_directories( ${CMAKE_SOURCE_DIR}/src/backend/common  ${KDE4_INCLUDES}  )

set(pgzrptcommon_LIB_SRCS
	krpos.cpp
	krsize.cpp
	krobjectdata.cpp
	krlabeldata.cpp
	krfielddata.cpp
	krtextdata.cpp
	krbarcodedata.cpp
	krimagedata.cpp
	krlinedata.cpp
	labelsizeinfo.cpp
	reportpageoptions.cpp
	parsexmlutils.cpp
)

kde4_add_library(pgzrptcommon SHARED ${pgzrptcommon_LIB_SRCS})

target_link_libraries(pgzrptcommon ${KDE4_KDECORE_LIBS} kdeui komain koproperty koodf)

set_target_properties(pgzrptcommon PROPERTIES VERSION 0.0.1 SOVERSION 0 )

install(TARGETS pgzrptcommon DESTINATION ${LIB_INSTALL_DIR})

