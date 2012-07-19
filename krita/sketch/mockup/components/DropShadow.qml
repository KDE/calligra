import QtQuick 1.1

Item {
    property real size: Constants.DefaultMargin;

    BorderImage {
        anchors.centerIn: parent;
        width: parent.width + 2 * parent.size;
        height: parent.height + 2 * parent.size;

        border { left: 16; right: 16; top: 16; bottom: 16; }

        source: ":/images/DropShadow.png"
    }
}