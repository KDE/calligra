import QtQuick 1.1
import Sailfish.Silica 1.0
import FileSystemModel 1.0

Page {
    id: page
    SilicaListView {
        id: listView
        anchors.fill: parent
//        header: PageHeader {
//            title: "Open File"
//        }
        //model: 20
        model: FileSystemModelItem {
            id: fileModel
            onRootPathChanged: {
                console.log("onRootPathChanged " + newPath)
                //pathLabel.text = newPath
            }
        }
        delegate: BackgroundItem {
            Column {
                //id: listItemContent
                x: theme.paddingLarge
                //y: 4
                spacing: 2
                Label {
                    id: titleLabel
                    font.weight: Font.Bold
                    text: fileName
                }
                Row {
                    id: columnRow
                    spacing: 10
                    Label {
                        id: detailLabel
                        font.weight: Font.Light
                        text: fileType + "   " + fileSize
                    }
                }
            }
            onClicked: {
                console.log("Clicked " + fileName)

//                if (fileIsDirectory) {
//                    if (model.hasModelChildren) {
//                        var idx = fileListView.model.modelIndex(index)
//                        fileListView.model.rootIndex = idx
//                    }
//                } else {
//                    var idx = fileListView.model.modelIndex(index)
//                    var role = fileModel.role('FilePathRole')
//                    var file = fileModel.data(idx, role)
//                    if (file != null && file != undefined) {
//                        filePage.fileClicked(file)
//                    }
//                }
            }
        }
    }

//    Component.onCompleted: {
//        //var directory = fileModel.homePath()
//        var directory = fileModel.documentsPath()
//        fileListView.model.rootIndex = filesVisualModel.model.setDirectory(directory)
//    }
}





