import QtQuick 1.1
import ":/components"

PageStack {
    id: base;
    initialPage: createNewListPage;
    clip: true;

    signal clicked();

    Component { id: createNewListPage; Page { CreateNewList { anchors.fill: parent; onClicked: pageStack.push( createNewPage ); } } }
    Component { id: createNewPage; Page {
        Column {
            anchors.fill: parent;
            Item {
                width: parent.width;
                height: Constants.GridHeight;
                Image {
                    id: titleImage;
                    anchors.left: parent.left;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.margins: Constants.DefaultMargin;
                    source: ":/images/document-new.png"
                }
                Label {
                    anchors.left: titleImage.right;
                    anchors.margins: Constants.DefaultMargin;
                    anchors.verticalCenter: parent.verticalCenter;
                    text: "Custom Size";
                    font.pixelSize: Constants.LargeFontSize;
                }
            }
            TextField { width: parent.width; height: Constants.GridHeight; placeholder: "Width" }
            TextField { width: parent.width; height: Constants.GridHeight; placeholder: "Height" }
            TextField { width: parent.width; height: Constants.GridHeight; placeholder: "Resolution" }
            Item { width: parent.width; height: Constants.GridHeight; }
            Row {
                width: parent.width;
                Button {
                    width: parent.width / 2;
                    height: Constants.GridHeight;
                    color: Constants.Theme.SecondaryColor;
                    text: "Cancel";
                    textColor: "white";
                    onClicked: pageStack.pop();
                }
                Button {
                    width: parent.width / 2;
                    height: Constants.GridHeight;
                    color: Constants.Theme.TertiaryColor;
                    text: "Create";
                    textColor: "white";
                    onClicked: base.clicked();
                }
            }
        }
    }
    }
}
