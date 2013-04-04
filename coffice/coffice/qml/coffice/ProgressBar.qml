import QtQuick 1.1

Item {
    id: progressbar
    width: parent.width;
    clip: true

    property int minimum: 0
    property int maximum: 100
    property int value: 0
    property color color: "#a0a0a0"

    Rectangle {
        id: border
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        height: Math.min(parent.height, 10)
        color: "transparent"
    }

    Rectangle {
        id: highlight
        property int widthDest: ( ( border.width * ( value - minimum ) ) / ( maximum - minimum ) - 4 )
        width: highlight.widthDest
        radius: 4
        border.width: 1
        border.color: "#6666ff"
        color: "#c0c0c0"

        Behavior on width {
            SmoothedAnimation {
                velocity: 1200
            }
        }

        anchors {
            left: border.left
            top: border.top
            bottom: border.bottom
            margins: 1
        }
    }
}
