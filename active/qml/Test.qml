import QtQuick 1.1
import org.calligra.CalligraComponents 0.1 as Calligra

Rectangle {
    color: "red"
    width: 100
    height: 200

    Calligra.TextDocumentCanvas {
        id: canvas
        source: "/home/shaan/Documents/all/Resume.odt"
        anchors.fill: parent
        zoomMode: Calligra.TextDocumentCanvas.ZOOM_WIDTH

        MouseArea {
            anchors.fill: parent
            onClicked: parent.searchTerm = 'Shantanu'
        }

        ListView {
            anchors.fill: parent
            model: canvas.documentModel
            delegate: Image { source: decoration }
        }
    }
}
