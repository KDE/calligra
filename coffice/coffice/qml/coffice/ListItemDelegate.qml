import QtQuick 1.1

Item {
    id: listItem
    width: parent.width
    height: listItemContent.implicitHeight + 5

    property alias titleText: titleLabel.text
    property alias detailText: detailLabel.text

    signal itemClicked(variant item)

    Rectangle {
        anchors.fill: parent
        visible: mouseArea.pressed
        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "#d0d0d0"
            }
            GradientStop {
                position: 1.0
                color: "#f0f0f0"
            }
        }
    }

    Column {
        id: listItemContent
        x: 5
        Label {
            id: titleLabel
            font.weight: Font.Bold
            //font.pointSize: 16
        }
        Row {
            id: columnRow
            spacing: 10
            Label {
                id: detailLabel
                font.weight: Font.Light
                //font.pointSize: 16
            }
        }
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: listItem.itemClicked(listItem)
    }
}
