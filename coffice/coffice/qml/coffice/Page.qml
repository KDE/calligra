import QtQuick 1.1

Rectangle {
    id: root
    color: "transparent"
    visible: false
    anchors.fill: parent

    signal activating()
    signal activated()
    signal deactivating()
    signal deactivated()
}
