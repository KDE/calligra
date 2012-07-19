import QtQuick 1.1
import "components"
import "panels"

Page {
    Rectangle {
        anchors.fill: parent;
        color: "grey"

        DropShadow {
            anchors.centerIn: parent;
            width: parent.width / 2;
            height: parent.height / 2;

            Rectangle {
                anchors.fill: parent;
                color: "white";

                MouseArea {
                    anchors.fill: parent;
                    onClicked: pageStack.pop();
                }
            }
        }

        MenuPanel {
            anchors.bottom: parent.bottom;

            width: parent.width;
            z: 10;

            onButtonClicked: {
                switch( button ) {
                    case "new":
                        newPanel.collapsed = !newPanel.collapsed;
                        break;
                    case "settings":
                        pageStack.push( settingsPage );
                        break;
                    case "share":
                        pageStack.push( sharePage );
                        break;
                }
            }
        }

        CreateNewPanel {
            id: newPanel;
            anchors.left: parent.left;
            width: Constants.GridWidth * 4;
            height: Constants.GridHeight * 7;
        }
    }

    Component { id: sharePage; SharePage { } }
    Component { id: settingsPage; SettingsPage { } }
}