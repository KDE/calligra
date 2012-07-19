import QtQuick 1.1

Rectangle {

    property alias text: title.text;
    property alias leftArea: left.children;
    property alias rightArea: right.children;

    height: Constants.GridHeight;
    width: Constants.GridWidth * Constants.GridColumns;

    color: Constants.Theme.MainColor;

    Row {
        id: left;

        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
    }

    Label {
        id: title;
        anchors.centerIn: parent;

        font.pixelSize: Constants.HugeFontSize;
        font.bold: true;
        color: "white";
    }

    Row {
        id: right;

        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
    }
}