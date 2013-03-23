import QtQuick 1.1

Item {
    id: progressbar

    property int minimum: 0
    property int maximum: 100
    property int value: 0
    property color color: "#a0a0a0"

    width: parent.width;
    height: 23
    clip: true

//    Rectangle {
//        id: border
//        anchors.fill: parent
//        anchors.bottomMargin: 1
//        anchors.rightMargin: 1
//        color: "transparent"
//        border.width: 1
//        border.color: parent.color
//    }

    Rectangle {
        id: highlight
        property int widthDest: ( ( progressbar.width * ( value - minimum ) ) / ( maximum - minimum ) - 4 )
        width: highlight.widthDest
        radius: 4

        Behavior on width {
            SmoothedAnimation {
                velocity: 1200
            }
        }

        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
            margins: 2
        }

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#909090"
            }
            GradientStop {
                position: 1.0
                color: "transparent"
            }
        }
    }
}
