import QtQuick 1.1

Item {
    id: listItem
    width: parent.width
    height: listItemContent.implicitHeight + 10

    property alias titleText: titleLabel.text
    property alias detailText: detailLabel.text

    signal itemClicked(variant item)

    Rectangle {
        anchors.fill: parent
        visible: mouseArea.pressed
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#000030"
            }
            GradientStop {
                position: 1.0
                color: "#000060"
            }
        }
    }

    Column {
        id: listItemContent
        x: 5
        y: 4
        spacing: 2
        Label {
            id: titleLabel
            font.weight: Font.Bold
        }
        Row {
            id: columnRow
            spacing: 10
            Label {
                id: detailLabel
                font.weight: Font.Light
                color: "#cacaca"
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: listItem.itemClicked(listItem)
    }
}
