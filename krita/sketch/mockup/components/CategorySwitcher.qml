import QtQuick 1.1

Row {
    id: base;
    property variant categories;

    ListView {
        id: view;
        width: Constants.GridWidth * 4 - 1;
        height: parent.height;
        clip: true;

        delegate: Item {
            width: parent.width;
            height: Constants.GridHeight;
            Label {
                anchors {
                    left: parent.left;
                    leftMargin: Constants.DefaultMargin;
                    verticalCenter: parent.verticalCenter;
                }
                text: modelData.name;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if( view.currentIndex != index ) {
                        view.currentIndex = index;
                        pageStack.replace( modelData.page );
                    }
                }
            }
        }

        highlight: DropShadow {
            width: parent.width;
            height: Constants.GridHeight;

            Rectangle { anchors.fill: parent; color: Constants.Theme.HighlightColor; }
        }

        model: base.categories;
    }

    Divider { height: parent.height; }

    PageStack {
        id: pageStack;

        width: Constants.GridWidth * 8 - 1;
        height: parent.height;

        initialPage: categories[0].page;
    }
}