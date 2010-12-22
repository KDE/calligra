
if(QCA2_FOUND)
    add_definitions( -DHAVE_QCA2 )
    include_directories(${QCA2_INCLUDE_DIR})
endif(QCA2_FOUND)

add_subdirectory( sidewinder )
add_subdirectory( import )
add_subdirectory( export )

include_directories( ${CMAKE_SOURCE_DIR}/kspread ${KDE4_INCLUDES}  )

