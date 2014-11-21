import QtQuick 1.1

Rectangle {
    id: i_infobanner; anchors.fill: parent; color: "transparent"; visible: false; z:42
    property alias text: banner_text.text

    Rectangle {
        x: 20; y: 20; width: parent.width-40; height: banner_text.paintedHeight + 10;
        color: Qt.rgba(128,128,128,0.9);
        radius: 20; clip: true
        Text {
            id: banner_text
            //anchors.fill: parent
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 10
            anchors.topMargin: 10
            font: Settings.theme.font("application");
            wrapMode: Text.WordWrap
        }
    }
    onVisibleChanged: {
        //console.log("Infobanneri visible changes")
        if(visible == true) closeTimer.start();
    }
    Timer {
        id: closeTimer; interval: 3000; running: false; repeat: false; onTriggered: i_infobanner.visible = false
    }
}
