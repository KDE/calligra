import QtQuick 1.1

Rectangle {
    id: buttonBar
    anchors {
        top: parent.top
        left: parent.left
        right: parent.right
    }
    height: placeholder.height + placeholder.anchors.margins*2
    color: "#000000"

    gradient: Gradient {
        GradientStop {
            position: 0.0
            color: "#808080"
        }
        GradientStop {
            position: 1.0
            color: "#191919"
        }
    }

    default property alias contents: placeholder.children
    Rectangle {
        id: placeholder
        color: "transparent"
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
            margins: 5
        }
        height: childrenRect.height
    }
}
