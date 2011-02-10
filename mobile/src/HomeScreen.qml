import QtQuick 1.0 as QML

QML.Rectangle {
    id: rootRect

    width: 800; height: 600;
    gradient: QML.Gradient {
         QML.GradientStop { position: 0.0; color: "#808080" }
         QML.GradientStop { position: 1.0; color: "#303030" }
    }

    QML.Row {
        id: docChoices
        
        anchors.centerIn: parent
        
        spacing: 10

        Button {
            id: button1
            
            imageSource: "qrc:///images/words.png"
            text: "Document"
            width: 100; height: 100
        }
        Button {
            id: button2
            
            imageSource: "qrc:///images/tables.png"
            text: "Spreadsheet"
            width: 100; height: 100
        }
        Button {
            id: button3
            
            imageSource: "qrc:///images/stage.png"
            text: "Presentation"
            width: 100; height: 100

            QML.MouseArea {
                anchors.fill: parent
                onPressed: rootRect.state = "presentation"
            }
        }
    }

    PresentationTemplatesView {
        id: presentationTemplatesView
        height: 400; width: 800;
        
        anchors.centerIn: parent
        visible: false
    }

    states: [
        QML.State {
            name: "presentation"
            QML.PropertyChanges { target: docChoices; visible: false }
            QML.PropertyChanges { target: presentationTemplatesView; visible: true }
        }
    ]
}
