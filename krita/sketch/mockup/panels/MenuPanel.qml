import QtQuick 1.1
import ":/components"

Item {
    id: base;

    property bool collapsed: true;

    signal buttonClicked( string button );

    height: Constants.GridHeight;

    DropShadow {
        id: shadow;
        anchors.fill: parent;
    }

    Rectangle {
        id: background;
        anchors.fill: parent;
        color: Constants.Theme.MainColor;

        Row {
            Button {
                id: newButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/document-new.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "new" );
            }
            Button {
                id: openButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/document-open.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "open" );
            }
            Button {
                id: saveButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                text: "Save";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "save" );
            }
            Button {
                id: saveAsButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                text: "Save As";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "saveAs" );
            }
        }

        Row {
            anchors.horizontalCenter: parent.horizontalCenter;

            Button {
                id: undoButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                text: "Undo";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "undo" );
            }
            Button {
                id: redoButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                text: "Redo";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "redo" );
            }
        }

        Row {
            anchors.right: parent.right;

            Button {
                id: shareButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                text: "Share";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "share" );
            }
            Button {
                id: settingsButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                text: "Settings";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "settings" );
            }
        }
    }

    Item {
        anchors.bottom: parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: Constants.GridWidth * 2;
        height: Constants.GridHeight / 2;

        DropShadow {
            anchors.fill: parent;
        }

        Rectangle {
            anchors.fill: parent;
            color: Constants.Theme.MainColor;

            Label {
                anchors.centerIn: parent;
                text: "Menu";
                color: "white";
            }
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: base.collapsed = !base.collapsed;
        }
    }

    states: State {
        name: "collapsed";
        when: base.collapsed;

        PropertyChanges { target: base; height: 0 }
        PropertyChanges { target: shadow; opacity: 0 }
        PropertyChanges { target: background; opacity: 0 }
    }

    transitions: Transition {
        NumberAnimation { duration: 250; properties: "height,opacity"; }
    }
}