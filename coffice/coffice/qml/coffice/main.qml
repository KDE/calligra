import QtQuick 1.1
import DocumentView 1.0

Rectangle {
    width: 360
    height: 360

    Row {
        Text {
            text: qsTr("Hello World")
            anchors.centerIn: parent
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
            //anchors.fill: parent.anchors
        }
    }
}
