include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  )

set(kpr_shapeanimation_example_PART_SRCS Plugin.cpp )

add_library(kpr_shapeanimation_example MODULE ${kpr_shapeanimation_example_PART_SRCS})


target_link_libraries(
    kpr_shapeanimation_example 
    calligrastageprivate
    
    Qt6::Core
    )

install(TARGETS kpr_shapeanimation_example  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrastage/shapeanimations)
