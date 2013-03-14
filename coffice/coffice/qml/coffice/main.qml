import QtQuick 1.1
import DocumentView 1.0

Rectangle {
    width: 800
    height: 600
    color: "#c0c0c0"

    Flickable {
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        contentHeight: buttonBar.height + documentViewItem.implicitHeight
        clip: true

        Row {
            id: buttonBar
            Button {
                text: "Open"
                onClicked: {
                    documentViewItem.openFileWithDialog()
                }
            }
            /*
            Button {
                text: "Save As"
                onClicked: {
                }
            }
            */
        }

        /*
        MouseArea {
            anchors.fill: parent
            onClicked: {
                Qt.quit();
            }
        }
        */

        DocumentViewItem {
            id: documentViewItem
            anchors.top: buttonBar.bottom
        }
    }

    Component.onCompleted: {
        //documentViewItem.openFile("/home/snoopy/test.odt")
    }
}
