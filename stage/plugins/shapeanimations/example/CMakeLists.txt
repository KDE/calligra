include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  )

set(kpr_shapeanimation_example_PART_SRCS Plugin.cpp )

add_library(kpr_shapeanimation_example MODULE ${kpr_shapeanimation_example_PART_SRCS})

calligrastage_shapeanimation_desktop_to_json(kpr_shapeanimation_example kpr_shapeanimation_example.desktop)

target_link_libraries(
    kpr_shapeanimation_example 
    calligrastageprivate
    
    Qt5::Core
    )

install(TARGETS kpr_shapeanimation_example  DESTINATION ${PLUGIN_INSTALL_DIR}/calligrastage/shapeanimations)
