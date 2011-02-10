import QtQuick 1.0 as QML

QML.Rectangle {
    property alias text: buttonText.text
    property alias imageSource: buttonImage.source
    
    gradient: QML.Gradient {
         QML.GradientStop { position: 0.0; color: "#DCDCDC" }
         QML.GradientStop { position: 1.0; color: "#ABABAB" }
    }
    
    radius: 10

    QML.Column {
        anchors.fill: parent
        anchors.margins: 10

        QML.Image {
            id: buttonImage
            
            width: parent.width
            height: parent.height*0.75
        }

        QML.Text {
            id: buttonText
            
            color: "white"            
            width: parent.width
            height: parent.height*0.25
            horizontalAlignment: QML.Text.AlignHCenter
            verticalAlignment: QML.Text.AlignVCenter
        }
    }
    
    
}