add_subdirectory( tests )

include_directories( ${KOSTEP_INCLUDES} ${QT_INCLUDES} )

########### libkostep ###############

set(kostep_LIB_SRCS
	StepDeleteTextBlockStep.cpp
	StepDeleteTextStep.cpp
	StepAddTextBlockStep.cpp
	StepAddTextStep.cpp
        StepStepBase.cpp
        StepStepBase_p.cpp
        StepStepStack.cpp
        StepStepStack_p.cpp
        StepStepLocation.cpp
        StepStepLocation_p.cpp
)

kde4_add_library(kostep SHARED ${kostep_LIB_SRCS})
target_link_libraries(kostep ${QT_QTCORE_LIBRARY})
target_link_libraries(kostep ${QT_QTGUI_LIBRARY})

#target_link_libraries(kostep ${KDE4_KIO_LIBS} ${QT_QTXML_LIBRARY})
#target_link_libraries(kostep LINK_INTERFACE_LIBRARIES ${KDE4_KIO_LIBS})

set_target_properties(kostep PROPERTIES
    VERSION ${GENERIC_CALLIGRA_LIB_VERSION} SOVERSION ${GENERIC_CALLIGRA_LIB_SOVERSION}
)
install(TARGETS kostep ${INSTALL_TARGETS_DEFAULT_ARGS} )


install( FILES
	kostep_export.h
        StepStepBase.h
        StepStepBase_p.h
        StepStepStack.h
        StepStepStack_p.h
        StepStepLocation.h
        StepStepLocation_p.h
        StepAddTextStep.h
        StepAddTextStep_p.h
        StepAddTextBlockStep.h
        StepAddTextBlockStep_p.h
        StepDeleteTextStep.h
        StepDeleteTextStep_p.h
	StepDeleteTextBlockStep.h
	StepDeleteTextBlockStep_p.h
DESTINATION ${INCLUDE_INSTALL_DIR} COMPONENT Devel)

