import QtQuick 1.1

Item {
    id: base;

    signal clicked();

    property alias image: icon.source;
    property alias color: fill.color;
    property alias text: label.text;
    property alias textColor: label.color;

    property color highlightColor: color;

    width: Constants.GridWidth;
    height: Constants.GridHeight;

    DropShadow {
        id: shadow;
        anchors.fill: parent;
        anchors.margins: Constants.DefaultMargin;
    }

    Rectangle {
        id: fill;
        anchors.fill: parent;
        anchors.margins: Constants.DefaultMargin;

        color: "white";

        Image {
            id: icon;
            anchors.fill: parent;
            anchors.margins: Constants.DefaultMargin;
            fillMode: Image.PreserveAspectFit;
        }

        Label {
            id: label;
            anchors.centerIn: parent;
        }
    }

    MouseArea {
        id: mouse;
        anchors.fill: parent;
        onClicked: base.clicked();
    }

    states: State {
        name: "pressed";
        when: mouse.pressed;

        PropertyChanges { target: shadow; size: Constants.DefaultMargin / 2; }
        PropertyChanges { target: fill; color: base.highlightColor; }
    }

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { properties: "size"; duration: 50; }
            ColorAnimation { duration: 50; }
        }
    }
}