import QtQuick 1.1
import "components"

Page {
    DropShadow {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        height: Constants.GridHeight;
        z: 10;

        Header {
            text: "Settings";

            leftArea: Button {
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                highlightColor: Constants.Theme.HighlightColor;
                text: "<";
                onClicked: pageStack.pop();
            }
        }
    }

    CategorySwitcher {
        anchors.bottom: parent.bottom;
        height: Constants.GridHeight * 7;

        categories: [ { name: "Display", page: displayPage }, { name: "Pressure", page: pressurePage } ];
    }

    Component { id: displayPage; Page { Label { anchors.centerIn: parent; text: "Display" } } }
    Component { id: pressurePage; Page { Label { anchors.centerIn: parent; text: "Pressure" } } }
}