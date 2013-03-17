import QtQuick 1.1
import org.calligra.CalligraComponents 0.1 as Calligra

Flickable {
    Calligra.SpreadsheetCanvas {
        id: canvas
        source: "/home/shaan/Documents/all/a-test-spreadsheet.ods"
        anchors.fill: parent
        //zoomMode: Calligra.TextDocumentCanvas.ZOOM_WIDTH

//         Text {
//             anchors.centerIn: parent
//             text: "SIZEE " + parent.documentSize.width
//         }

//         MouseArea {
//             anchors.fill: parent
//             onClicked: parent.searchTerm = 'Shantanu'
//         }
// 
//         ListView {
//             anchors.fill: parent
//             model: canvas.documentModel
//             delegate: Image { source: decoration }
//         }
    }
}
