import QtQuick 1.1

Rectangle {
    id: slider
    width: parent.width
    height: 24
    color: "transparent"

    property real minimum: 0
    property real maximum: 100
    property real value: 50
    signal valueChangedBegin()
    signal valueChangedEnd()

    property int xMax: width - handle.width - 4
    onXMaxChanged: updatePosition()
    onMinimumChanged: updatePosition()
    onMaximumChanged: updatePosition()
    onValueChanged: updatePosition()
    //onWidthChanged: updatePosition()
    //onHeightChanged: updatePosition()

    function updatePosition() {
        if (maximum > minimum) {
            var xpos = 2 + (value - minimum) * slider.xMax / (maximum - minimum)
            xpos = Math.min(xpos, width - handle.width - 2)
            xpos = Math.max(xpos, 2)
            handle.x = xpos
        } else {
            handle.x = 2
        }
    }

    function setPosition(xpos) {
        value = (maximum - minimum) * xpos / slider.xMax + minimum
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            valueChangedBegin()
            setPosition(mouseX)
        }
        onReleased: {
            slider.valueChangedEnd()
        }
    }

    Rectangle {
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        height: 3
        color: "#606060"
        border.width:  1
        border.color:"#303030"
    }

    Rectangle {
        id: handle
        y: 1
        width: height * 1.5
        height: Math.max(0, slider.height - 2)
        radius: height / 2.0
        //border.width: 1
        //border.color: "#a0a0a0"
        smooth: true

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: mouseArea.pressed ? "#000030" : "#909090"
            }
            GradientStop {
                position: 1.0
                color: mouseArea.pressed ? "#000060" : "#404040"
            }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
            drag.target: parent
            drag.axis: Drag.XAxis
            drag.minimumX: 2
            drag.maximumX: slider.xMax + 2
            onPositionChanged: setPosition(handle.x - 2)
            onPressed: slider.valueChangedBegin()
            onReleased: slider.valueChangedEnd()
        }
    }
}
