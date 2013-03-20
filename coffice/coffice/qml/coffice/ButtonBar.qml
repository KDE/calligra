import QtQuick 1.1

Rectangle {
    id: buttonBar
    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
        margins: 5
    }
    color: "#f0f0f0"
    border.width: 10
    border.color: color
    height: childrenRect.height
}
