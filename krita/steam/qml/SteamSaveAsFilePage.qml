/* This file is part of the KDE project
 * Copyright (C) 2014 Stuart Dickson <stuartmd@kogmbh.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */
import QtQuick 1.1
import org.krita.steam 1.0
import org.krita.sketch 1.0
import org.krita.sketch.components 1.0

Column {
    id: base
    property color steamBorderColor: "#383635"
    property color steamBackLightColor: "#3E3D3C"
    property color steamBackDarkColor: "#262626"
    property color steamHeadingTextColor: "#C7C4C2"
    property color steamActiveHeadingTextColor: "#FFFFFF"

    signal finished(string file, string type);

    width: 640
    height: 480

    KisSteamComponent {
        id: steamComponent;
    }

    Component.onCompleted: {
        Settings.theme.useScreenGeometry = true
        Constants.setGrid(4, 11)
        Constants.useScreenGeometry = true
        Constants.setGridWidth(width / Constants.GridColumns)
        Constants.setGridHeight(height / Constants.GridRows)
    }
    onWidthChanged: Constants.setGridWidth(width / Constants.GridColumns)
    onHeightChanged: Constants.setGridHeight(height / Constants.GridRows)

    Rectangle {
        id: header
        height: Constants.GridHeight
        width: parent.width
        z: 2
        radius: Constants.DefaultMargin

        gradient: Gradient {
            GradientStop {
                position: 0
                color: Settings.theme.color("panels/openImage/header/start")
            }

            GradientStop {
                position: 1
                color: Settings.theme.color("panels/openImage/header/stop")
            }
        }

        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            height: Constants.DefaultMargin
            color: Settings.theme.color("panels/openImage/header/stop")
        }

        Shadow {
            width: parent.width
            height: Constants.GridHeight / 8
            anchors.top: parent.bottom
        }

        Label {
            anchors {
                left: parent.left
                leftMargin: Constants.DefaultMargin
                verticalCenter: parent.verticalCenter
            }
            text: "Save to Cloud"
            font.pixelSize: Constants.LargeFontSize
            color: Settings.theme.color("panels/openImage/header/text")
        }

        Image {
            anchors {
                right: parent.right
                rightMargin: Constants.DefaultMargin
                verticalCenter: parent.verticalCenter
            }
            width: Constants.GridHeight
            height: Constants.GridHeight
            source: Settings.theme.icon("alert")
        }
    }

    CapacityBar {
        id: capacityBar
        width: header.width

        anchors {
            leftMargin: Constants.DefaultMargin
            rightMargin: Constants.DefaultMargin
            topMargin: Constants.DefaultMargin
            bottomMargin: Constants.DefaultMargin
        }

        height: Constants.GridHeight * 0.2

        maxCapacity: storageListModel.maxCapacity
        usedCapacity: storageListModel.usedCapacity
        oldItemSize: 0
        newItemSize: currentDocumentFileSize
    }

    Rectangle {

        anchors {
            top: capacityBar.bottom
            left: parent.left
            right: parent.right
            bottom: footer.top
        }

        gradient: Gradient {
            GradientStop {
                position: 0.0
                color: "black"
            }
            GradientStop {
                position: 0.2
                color: steamBorderColor
            }
            GradientStop {
                position: 1.0
                color: steamBorderColor
            }
        }

        StorageListModel {
            id: storageListModel
            kritaSteamClient: SteamClientInstance

            onFilenameFound: {
                filesList.selectItem(index)
            }
        }

        Rectangle {
            id: steamContents
            anchors {
                fill: parent
            }

            gradient: Gradient {
                GradientStop {
                    position: 0.0
                    color: base.steamBackLightColor
                }
                GradientStop {
                    position: 1.0
                    color: base.steamBackDarkColor
                }
            }

            Item {
                anchors.fill: parent

                ListView {
                    id: filesList

                    property int selectedFileIndex: -1
                    property string selectedFileName: ""
                    property string selectedFileSizeString: ""

                    anchors {
                        left: parent.left
                        top: parent.top
                        bottom: parent.bottom
                    }
                    width: parent.width * 0.6
                    clip: true

                    delegate: ListItem {
                        title: model.fileName
                        description: ""
                        width: filesList.width

                        visibleHorizontalMargin: Constants.DefaultMargin
                        visibleVerticalMargin: Constants.DefaultMargin

                        onClicked: {
                            if (filesList.selectedFileIndex != index) {
                                filesList.selectItem(index)
                            } else {
                                filesList.selectItem(-1)
                            }
                            // Update file display
                        }
                    }

                    model: storageListModel

                    ScrollDecorator {
                    }

                    onCountChanged: {
                    }

                    function selectText(filename, type) {
                        model.requestIndexForFilename("%1.%2".arg(
                                                          filename).arg(type))
                    }

                    function selectItem(index) {
                        if (index >= 0) {
                            filesList.selectedFileIndex = index
                            filesList.selectedFileName = model.get(
                                        index).fileName
                            filesList.selectedFileSizeString = model.get(
                                        index).fileSizeMb
                        } else {
                            filesList.selectedFileIndex = -1
                            filesList.selectedFileName = ""
                            filesList.selectedFileSizeString = ""
                        }
                    }
                }

                Column {
                    id: fileInfoPane

                    anchors {
                        right: parent.right
                        top: parent.top
                        bottom: parent.bottom
                        left: filesList.right
                    }

                    Image {
                        id: fileImage
                        visible: (filesList.selectedFileIndex != -1)
                        anchors {
                            left: parent.left
                            leftMargin: Constants.DefaultMargin
                            right: parent.right
                            rightMargin: Constants.DefaultMargin
                            top: parent.top
                            topMargin: Constants.DefaultMargin
                        }
                        height: width
                        LayoutRectangle {
                        }
                    }

                    Label {
                        id: labelFileName
                        text: filesList.selectedFileName
                        color: "white"
                    }
                    Label {
                        id: labelFileSize
                        text: filesList.selectedFileSizeString
                        color: "white"
                    }
                    Label {
                        id: labelFileDate
                        text: "File date"
                        color: "white"
                    }
                    Label {
                        text: currentDocumentFileName
                        color: "red"
                    }
                    Label {
                        text: currentDocumentFileSize
                        color: "red"
                    }
                    Label {
                        text: "max: " + storageListModel.maxCapacity
                        color: "yellow"
                    }
                    Label {
                        text: "used: " + storageListModel.usedCapacity
                        color: "yellow"
                    }

                    Button {
                        id: buttonDelete
                        visible: (filesList.selectedFileIndex != -1)
                        text: "Delete"

                        onClicked: {

                            // Delete the current image
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: footer

        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        height: Constants.GridHeight
        color: Settings.theme.color("pages/save/footer")

        Row {
            anchors {
                fill: parent
                leftMargin: Constants.DefaultMargin
                rightMargin: Constants.DefaultMargin
            }

            PanelTextField {
                id: fileNameField

                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: Constants.DefaultMargin
                height: Constants.GridHeight * 0.75
                width: Constants.GridWidth * 2 - Constants.DefaultMargin

                placeholder: "File Name"

                onTextChanged: {
                    filesList.selectText(fileNameField.text, fileType.model.get(
                                             fileType.currentIndex).type)
                }
            }

            Rectangle {
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 1

                width: childrenRect.width
                height: childrenRect.height + Constants.GridHeight * 0.25 - 1

                radius: Constants.GridHeight * 0.25

                color: Settings.theme.color("pages/save/footer")

                ExpandingListView {
                    id: fileType

                    anchors.bottom: parent.bottom
                    anchors.bottomMargin: Constants.GridHeight * 0.125

                    buttonHeight: Constants.GridHeight * 0.75
                    width: Constants.GridWidth * 1.2

                    expandedHeight: Constants.GridHeight * 3
                    expandToTop: true

                    model: ListModel {
                        ListElement {
                            text: "Krita Image"
                            type: "kra"
                            mime: "application/x-krita"
                        }
                        ListElement {
                            text: "OpenRaster Image"
                            type: "ora"
                            mime: "image/openraster"
                        }
                        ListElement {
                            text: "PhotoShop Image"
                            type: "psd"
                            mime: "image/vnd.adobe.photoshop"
                        }
                        ListElement {
                            text: "PNG Image"
                            type: "png"
                            mime: "image/png"
                        }
                        ListElement {
                            text: "BMP Image"
                            type: "bmp"
                            mime: "image/bmp"
                        }
                        ListElement {
                            text: "JPEG Image"
                            type: "jpg"
                            mime: "image/jpeg"
                        }
                    }

                    onCurrentIndexChanged: {
                        filesList.selectText(fileNameField.text,
                                             fileType.model.get(
                                                 fileType.currentIndex).type)
                    }
                }
            }

            Button {
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right

                height: parent.height * 1.0
                width: parent.height * 1.0

                image: Settings.theme.icon("filesave")

                onClicked: {
                    if (fileNameField.text != "") {
                        var filePath = "%1/%2.%3".arg(filesList.model.path).arg(
                                    fileNameField.text).arg(
                                    fileType.model.get(
                                        fileType.currentIndex).type)
                        steamComponent.saveAs(filePath, fileType.model.get(fileType.currentIndex).mime);
                    }
                }
            }
        }
    }
}
