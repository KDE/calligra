import QtQuick 1.0 as QML

QML.ListView {
    model: PresentationTemplatesModel { }
    orientation: QML.ListView.Horizontal
    focus: true
    spacing: 20
    
    delegate: Button {
        height: 400; width: 400;
        text: name
        imageSource: "../templates/stage/" + name + ".png"
    }
}
