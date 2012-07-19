import QtQuick 1.1

Item {
    id: base;
    signal clicked();

    ListView {
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;

        height: Constants.GridHeight * 5;

        clip: true;

        delegate: Rectangle {
            width: Constants.GridWidth * 4;
            height: Constants.GridHeight;

            MouseArea {
                anchors.fill: parent;
                onClicked: base.clicked();
            }

            Button {
                id: thumbnail;
                onClicked: base.clicked();
                image: model.image;
            }

            Label {
                anchors {
                    top: parent.top;
                    topMargin: Constants.DefaultMargin;
                    left: thumbnail.right;
                    leftMargin: Constants.DefaultMargin;
                }

                text: model.name;
            }

            Label {
                anchors {
                    bottom: parent.bottom;
                    bottomMargin: Constants.DefaultMargin;
                    left: thumbnail.right;
                    leftMargin: Constants.DefaultMargin;
                }

                text: model.date;
                color: Constants.Theme.SecondaryTextColor;
                font.pixelSize: Constants.SmallFontSize;
            }
        }

        model: ListModel {
            ListElement { name: "Recent Image 00"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
            ListElement { name: "Recent Image 01"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
            ListElement { name: "Recent Image 02"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
            ListElement { name: "Recent Image 03"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
            ListElement { name: "Recent Image 04"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
        }
    }

    Item {
        anchors.bottom: parent.bottom;
        width: parent.width;
        height: Constants.GridHeight;

        MouseArea {
            anchors.fill: parent;
            onClicked: base.clicked();
        }

        Button {
            id: icon;
            onClicked: base.clicked();
            image: "../images/document-open.png";
        }

        Label {
            anchors {
                left: icon.right;
                leftMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }

            text: "Open Image";
        }
    }
}