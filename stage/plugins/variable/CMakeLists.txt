include_directories( ${KOMAIN_INCLUDES} ${KOPAGEAPP_INCLUDES} ${CMAKE_SOURCE_DIR}/stage/part ${Boost_INCLUDE_DIR} )

set(kprvariables_PART_SRCS 
    PresentationVariable.cpp
    PresentationVariableFactory.cpp
    Plugin.cpp
   )

add_library(kprvariables MODULE ${kprvariables_PART_SRCS})

kcoreaddons_desktop_to_json(kprvariables kprvariables.desktop)

target_link_libraries(
    kprvariables
    kotextlayout
    calligrastageprivate
    
    Qt5::Core
    )

install(TARGETS kprvariables  DESTINATION ${CALLIGRA_PLUGIN_INSTALL_DIR})
