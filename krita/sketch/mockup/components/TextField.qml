import QtQuick 1.1

Item {

    property alias text: input.text;
    property alias placeholder: placeholder.text;

    DropShadow {
        anchors.fill: parent;
        anchors.margins: Constants.DefaultMargin;
    }

    Rectangle {
        anchors.fill: parent;
        anchors.margins: Constants.DefaultMargin;

        Label {
            id: placeholder;
            anchors.left: parent.left;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.margins: Constants.DefaultMargin;
            color: Constants.Theme.SecondaryTextColor;
        }

        TextInput {
            id: input;
            anchors.left: parent.left;
            anchors.right: parent.right;
            anchors.verticalCenter: parent.verticalCenter;
            anchors.margins: Constants.DefaultMargin;
            font.pixelSize: Constants.DefaultFontSize;
        }
    }

    MouseArea {
        anchors.fill: parent;
        onClicked: input.focus = true;
    }

    states: State {
        name: "input";
        when: input.focus || input.text;

        PropertyChanges { target: placeholder; opacity: 0.5; }
        AnchorChanges { target: placeholder; anchors.left: undefined; anchors.right: parent.right }
    }

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { duration: 100; properties: "opacity"; }
            AnchorAnimation { duration: 100; }
        }
    }
}