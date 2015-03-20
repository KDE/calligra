
include_directories( ${KOMAIN_INCLUDES} ${FLAKE_INCLUDES} ${CMAKE_SOURCE_DIR}/kexi/core ${CMAKE_SOURCE_DIR}/kexi/main ${CMAKE_SOURCE_DIR}/kexi/kexidb)

set(kexirelationdesignshape_PART_SRCS  
kexirelationdesignshape.cpp
kexirelationdesignfactory.cpp
kexirelationdesignshapeplugin.cpp
kexirelationdesigntool.cpp
kexirelationdesigntoolfactory.cpp
simplefield.cpp
)

add_library(kexirelationdesignshape MODULE ${kexirelationdesignshape_PART_SRCS})

target_link_libraries(
kexirelationdesignshape
${CALLIGRADB_LIBS}
kexicore
keximain
komain


Qt5::Core
Qt5::Gui
)

install(TARGETS kexirelationdesignshape  DESTINATION ${PLUGIN_INSTALL_DIR})
install( FILES kexirelationdesignshape.desktop DESTINATION ${SERVICES_INSTALL_DIR}/calligra )

