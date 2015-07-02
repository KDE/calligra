include_directories(${CMAKE_SOURCE_DIR}/kexi/formeditor ${CMAKE_SOURCE_DIR}/kexi/core)

add_definitions(-DKEXI_FORMS_NO_LIST_WIDGET)

########### next target ###############

set(kformdesigner_containers_PART_SRCS containerfactory.cpp )


add_library(kformdesigner_containers MODULE ${kformdesigner_containers_PART_SRCS})



target_link_libraries(
    kformdesigner_containers
    kformdesigner
    kundo2


    Qt5::Core
    Qt5::Gui
    Qt5::Xml
    )

install(TARGETS kformdesigner_containers  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})


########### next target ###############

set(kformdesigner_stdwidgets_PART_SRCS stdwidgetfactory.cpp )

add_library(kformdesigner_stdwidgets MODULE ${kformdesigner_stdwidgets_PART_SRCS})



target_link_libraries(
    kformdesigner_stdwidgets
    kformdesigner

    KProperty

    Qt5::Gui
    Qt5::Xml
)

install(TARGETS kformdesigner_stdwidgets  DESTINATION ${KEXI_PLUGIN_INSTALL_DIR})
