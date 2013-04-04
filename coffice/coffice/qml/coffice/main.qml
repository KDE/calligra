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
            //top: buttonBar.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        FilePage {
            id: fileTab
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
                pageStack.push(fileTab, false)
            }
        }
    }

    Component.onCompleted: {
        pageStack.push(documentTab, true)
    }
}
