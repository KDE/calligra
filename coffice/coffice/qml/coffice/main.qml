import QtQuick 1.1

Rectangle {
    id: appWindow
    width: 800
    height: 600
    color: "#000000"

    PageStack {
        id: pageStack
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        Loader {
            id: fileTabLoader
            anchors.fill: parent
            focus: true
        }
        Connections {
            target: fileTabLoader.item
            onCloseClicked: {
                pageStack.pop(documentTab, false)
            }
            onFileClicked: {
                pageStack.pop(documentTab, false)
                documentTab.openFile(file)
            }
        }
        DocumentPage {
            id: documentTab
            onOpenClicked: {
                fileTabLoader.source = "FilePage.qml"
                pageStack.push(fileTabLoader.item, false)
            }
        }
    }

    Component.onCompleted: {
        pageStack.push(documentTab, true)
    }
}
