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
            text: "Share";

            leftArea: Button {
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                highlightColor: Constants.Theme.HighlightColor;
                text: "<";
                onClicked: pageStack.pop();
            }

            rightArea: Button {
                width: Constants.GridWidth * 2;
                height: Constants.GridHeight;
                color: Constants.Theme.TertiaryColor;
                textColor: "white";
                text: "Upload";
                onClicked: pageStack.pop();
            }
        }
    }

    CategorySwitcher {
        anchors.bottom: parent.bottom;
        height: Constants.GridHeight * 7;

        categories: [ { name: "DeviantArt", page: deviantArtPage }, { name: "MediaGoblin", page: mediaGoblinPage } ];

        Component { id: deviantArtPage; Page { Label { anchors.centerIn: parent; text: "DeviantArt" } } }
        Component { id: mediaGoblinPage; Page { Label { anchors.centerIn: parent; text: "MediaGoblin" } } }
    }
}