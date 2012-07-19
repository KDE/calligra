import QtQuick 1.1
import ":/components"

Item {
    id: base;
    property bool collapsed: true;

    DropShadow {
        id: panel;

        width: parent.width;
        height: parent.height;
        Rectangle {
            anchors.fill: parent;
            color: "white";
            clip: true;

            DropShadow {
                id: header;
                height: Constants.GridHeight;
                width: parent.width;
                z: 2;

                Rectangle {
                    anchors.fill: parent;

                    color: Constants.Theme.TertiaryColor;

                    Label {
                        anchors {
                            left: parent.left;
                            leftMargin: Constants.DefaultMargin;
                            verticalCenter: parent.verticalCenter;
                        }
                        text: "Create New";
                        font.pixelSize: Constants.LargeFontSize;
                        color: "white";
                    }
                }
            }

            CreateNewSection { anchors.top: header.bottom; }
        }
    }

    states: State {
        name: "collapsed";
        when: base.collapsed;
        PropertyChanges { target: panel; x: -base.width; opacity: 0; }
    }

    transitions: Transition {
        NumberAnimation { properties: "x,opacity"; }
    }
}