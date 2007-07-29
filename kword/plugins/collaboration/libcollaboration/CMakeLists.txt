include_directories( ${CMAKE_CURRENT_BINARY_DIR} )

add_subdirectory( ${CMAKE_CURRENT_SOURCE_DIR}/network/messages/tests )

set(kcollaborate_SRCS
    CollaborationManager.cpp
    UndoCommand.cpp
    UndoStack.cpp
    dialogs/ShareDialog.cpp
    dialogs/ConnectDialog.cpp
    dialogs/RemoteServiceListWidgetItem.cpp
    network/Session.cpp
    network/ClientSession.cpp
    network/ServerSession.cpp
    network/ServerConnection.cpp
    network/Url.cpp
    network/sockets/MessageTcpSocket.cpp
    network/sockets/MessageTcpServer.cpp
    network/messages/Generic.cpp
    network/messages/Hello.cpp
    network/messages/HelloAnswer.cpp
    network/messages/Update.cpp
    network/messages/UpdateAnswer.cpp
    network/messages/User.cpp
    network/messages/MessageFactory.cpp
)

set(kcollaborate_UIS
    dialogs/ShareDialog.ui
    dialogs/ConnectDialog.ui
)

add_definitions(${KDE4_ENABLE_EXCEPTIONS} -Wall)

kde4_add_ui_files(kcollaborate_SRCS ${kcollaborate_UIS} )

add_definitions(${KDE4_ENABLE_EXCEPTIONS} -Wall)

kde4_add_library(collaboration SHARED ${kcollaborate_SRCS} )
target_link_libraries(collaboration ${KDE4_KPARTS_LIBS} kdnssd)
set_target_properties(collaboration PROPERTIES VERSION 0.0.1 SOVERSION 0 )

install(FILES
    CollaborationManager.h
    kcollaborate_export.h
    UndoCommand.h
    UndoStack.h
DESTINATION ${INCLUDE_INSTALL_DIR}/libcollaboration)
install(FILES
    dialogs/ShareDialog.h
    dialogs/ConnectDialog.h
DESTINATION ${INCLUDE_INSTALL_DIR}/libcollaboration/dialogs)
install(FILES
    network/Session.h
    network/ClientSession.h
    network/ServerSession.h
    network/ServerConnection.h
    network/Url.h
DESTINATION ${INCLUDE_INSTALL_DIR}/libcollaboration/network)
install(FILES
    network/sockets/MessageTcpSocket.h
    network/sockets/MessageTcpServer.h
DESTINATION ${INCLUDE_INSTALL_DIR}/libcollaboration/sockets/network)
install(FILES
    network/messages/Generic.h
    network/messages/Hello.h
    network/messages/HelloAnswer.h
    network/messages/Update.h
    network/messages/MessageFactory.h
    network/messages/UpdateAnswer.h
    network/messages/User.h
DESTINATION ${INCLUDE_INSTALL_DIR}/libcollaboration/network/messages)
install(TARGETS collaboration DESTINATION ${LIB_INSTALL_DIR})
