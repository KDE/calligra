import QtQuick 1.1
import org.calligra.CalligraComponents 0.1 as Calligra

Rectangle {
    color: "red"
    width: 100
    height: 200

    Calligra.TextDocumentCanvas {
        source: "/home/shaan/Documents/all/Resume.odt"
        anchors.fill: parent
        zoomMode: Calligra.TextDocumentCanvas.ZOOM_WIDTH
    }
}
