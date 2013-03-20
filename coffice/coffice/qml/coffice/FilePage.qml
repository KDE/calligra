import QtQuick 1.1
import FileSystemModel 1.0

Page {
    id: filePage

    signal closeClicked()
    signal fileClicked(string file)

    ButtonBar {
        id: buttonBar
        Row {
            id: buttonRow
            spacing: 5
            Button {
                id: cancelButton
                //iconSource: "qrc:///images/cancel.png"
                text: "Cancel"
                //font.pointSize: 9
                onClicked: {
                    filePage.closeClicked()
                }
            }
            Label {
                id: pathLabel
                elide: Text.ElideLeft
                verticalAlignment: Text.AlignVCenter
                height: cancelButton.height
                width: buttonBar.width - buttonRow.spacing - cancelButton.width
            }
        }
    }

    VisualDataModel {
        id: filesVisualModel
        model: FileSystemModelItem {
            id: fileModel
            onRootPathChanged: {
                pathLabel.text = newPath
            }
        }
        delegate: ListItemDelegate {
            titleText: fileName
            detailText: fileType + "   " + fileSize
            onItemClicked: {
                if (fileIsDirectory) {
                    if (model.hasModelChildren) {
                        var idx = fileListView.model.modelIndex(index)
                        fileListView.model.rootIndex = idx
                    }
                } else {
                    var idx = fileListView.model.modelIndex(index)
                    var role = fileModel.role('FilePathRole')
                    var file = fileModel.data(idx, role)
                    if (file != null && file != undefined) {
                        filePage.fileClicked(file)
                    }
                }
            }
        }
    }

    ListItemDelegate {
        id: fileParentItem
        anchors {
            top: buttonBar.bottom
            left: parent.left
            right: parent.right
        }
        titleText: ".."
        detailText: qsTr("Parent Folder")
        onItemClicked: {
            var idx = fileListView.model.parentModelIndex()
            fileListView.model.rootIndex = idx
        }
    }

    ListView {
        id: fileListView
        anchors {
            top: fileParentItem.bottom
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }
        clip: true
        model: filesVisualModel
    }

    Component.onCompleted: {
        //var directory = fileModel.homePath()
        var directory = fileModel.documentsPath()
        fileListView.model.rootIndex = filesVisualModel.model.setDirectory(directory)
    }
}
