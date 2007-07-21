SET (kcollaborate_LIBRARY
	${CMAKE_BUILD_DIR}/lib
)

set(undoframework_testapp_SRCS
	commands.cpp
        diagramitem.cpp
        diagramscene.cpp
        main.cpp
        mainwindow.cpp	
)

set(undoframework_testapp_HDRS
	commands.h
	diagramitem.h
	diagramscene.h
	mainwindow.h
)

set(undoframework_testapp_RCCS
	undoframework.qrc
)


include_directories( ${kcollaborate_LIBRARY} )
include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/../ )

add_definitions(${KDE4_ENABLE_EXCEPTIONS} -Wall)

qt4_add_resources(undoframework_testapp_RCCS_SRCS ${undoframework_testapp_RCCS})

kde4_add_executable(undoframework_testapp ${undoframework_testapp_SRCS} ${undoframework_testapp_HDRS} ${undoframework_testapp_RCCS_SRCS})
target_link_libraries(undoframework_testapp ${KDE4_KDEUI_LIBS} ${KDE4_KPARTS_LIBS} collaboration )

install(TARGETS undoframework_testapp DESTINATION ${BIN_INSTALL_DIR})
