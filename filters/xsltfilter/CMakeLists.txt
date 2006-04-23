kde4_header()

include_directories(
    ${KOFFICECORE_INCLUDES}
    )

add_subdirectory( export )
add_subdirectory( import )

########### install files ###############

kde4_footer()
