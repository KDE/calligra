import QtQuick 1.1
import DocumentView 1.0

Rectangle {
    width: 800
    height: 600
    //color: "#c0c0c0"
    color: "#ffffff"

    Flickable {
        id: documentViewFlickable
        anchors {
            top: parent.top
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        contentHeight: buttonBar.height + documentViewItem.implicitHeight
        contentWidth: Math.max(documentViewItem.implicitWidth, parent.width)

//        onFlickStarted: documentViewItem.beginUpdate()
//        onFlickEnded: documentViewItem.endUpdate()
//        onMovementStarted: documentViewItem.beginUpdate()
//        onMovementEnded: documentViewItem.endUpdate()

        clip: true

        Rectangle {
            id: buttonBar
            x: 10
            y: 10
            width: buttonRow.implicitWidth
            height: buttonRow.implicitHeight


            Row {
                id: buttonRow
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
                        documentViewItem.saveFileWithDialog()
                    }
                }
                Button {
                    text: "Quit"
                    onClicked: {
                        Qt.quit()
                    }
                }
                */
            }
        }

        DocumentViewItem {
            id: documentViewItem
            anchors.top: buttonBar.bottom
            Component.onCompleted: {
                documentViewItem.multiTouchBegin.connect( function() { documentViewFlickable.interactive = false } )
                documentViewItem.multiTouchEnd.connect( function() { documentViewFlickable.interactive = true } )
            }
        }
    }

    Component.onCompleted: {
        //documentViewItem.openFile("/home/snoopy/test.odt")
        documentViewItem.openFile("/storage/sdcard0/Download/test2.odt")
    }
}
