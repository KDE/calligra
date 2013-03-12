import QtQuick 1.1
import DocumentView 1.0

Rectangle {
    width: 600
    height: 400

    /*
    Row {
        Text {
            text: qsTr("Hello World")
            anchors.centerIn: parent
        }
    */
        /*
        MouseArea {
            anchors.fill: parent
            onClicked: {
                Qt.quit();
            }
        }
        */

        DocumentViewItem {
            id: document
            anchors.fill: parent.anchors
        }
    /*
    }
    */

    Component.onCompleted: {
        console.log(">>>>>>>>>>>>>>>>> LOAD DOCUMENT START")
        document.openFile("/home/snoopy/test.odt")
        console.log(">>>>>>>>>>>>>>>>> LOAD DOCUMENT END")
    }
}
