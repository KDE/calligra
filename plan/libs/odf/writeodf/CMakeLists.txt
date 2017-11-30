set(RNGFILE ${RNG_SOURCE_DIR}/OpenDocument-v1.2-cs01-schema-calligra.rng)
set(RNGHEADERDIR ${CMAKE_CURRENT_BINARY_DIR})

#message(STATUS "RNG 'plan_rng2cpp' custom command: ${RNGFILE} to ${RNGHEADERDIR}")
add_custom_command(
    OUTPUT ${RNGHEADERDIR}/writeodf.h
    COMMAND plan_rng2cpp
    ARGS ${RNGFILE} ${RNGHEADERDIR}
    DEPENDS plan_rng2cpp ${RNGFILE}
    WORKING_DIRECTORY ${RNGHEADERDIR}
)
add_custom_target(plan_writeodf.h-target DEPENDS writeodf.h)
