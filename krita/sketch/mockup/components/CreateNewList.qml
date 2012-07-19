import QtQuick 1.1

ListView {
    id: base;

    signal clicked();

    delegate: Rectangle {
        width: Constants.GridWidth * 4;
        height: Constants.GridHeight;

        Button {
            id: thumbnail;

            image: model.image;

            onClicked: base.clicked();
        }

        Label {
            anchors {
                left: thumbnail.right;
                leftMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }

            text: model.name;
        }
    }

    model: ListModel {
        ListElement { name: "Blank Image (A4)"; image: "../images/document-new.png" }
        ListElement { name: "Blank Image (A3)"; image: "../images/document-new.png" }
        ListElement { name: "Custom Size"; image: "../images/document-new.png" }
        ListElement { name: "From Clipboard"; image: "../images/edit-paste.png" }
        ListElement { name: "From Camera"; image: "../images/camera-web.png" }
    }
}