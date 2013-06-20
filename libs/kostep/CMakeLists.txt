add_subdirectory( tests )

include_directories( ${KOSTEP_INCLUDES} ${QT_INCLUDES} )

########### libkostep ###############

set(kostep_LIB_SRCS
	StepStepBase.cpp
	StepStepStack.cpp
	StepStepStack_p.cpp
)

kde4_add_library(kostep SHARED ${kostep_LIB_SRCS})
target_link_libraries(kostep ${QT_QTCORE_LIBRARY})

set_target_properties(kostep PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kostep ${INSTALL_TARGETS_DEFAULT_ARGS} )


install( FILES
	StepStepBase.h
	StepStepStack.h
	StepStepStack_p.h
DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

