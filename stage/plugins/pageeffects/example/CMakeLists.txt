include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES}  )

set(kpr_pageeffect_example_PART_SRCS Plugin.cpp )

add_library(kpr_pageeffect_example MODULE ${kpr_pageeffect_example_PART_SRCS})

target_link_libraries(kpr_pageeffect_example calligrastageprivate flake )

install(TARGETS kpr_pageeffect_example  DESTINATION ${KDE_INSTALL_PLUGINDIR}/calligrastage/pageeffects)
