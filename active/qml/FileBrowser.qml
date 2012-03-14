/*
 *   Copyright 2011 Sebastian Kügler <sebas@kde.org>
 *   Copyright 2011 Shantanu Tushar <jhahoneyk@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 1.0
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.graphicslayouts 4.7 as GraphicsLayouts
import Qt.labs.folderlistmodel 1.0

Rectangle {
    PlasmaCore.Theme {
        id: theme
    }

    color: "white"

    Button {
        id: back
        drawBackground: false
        imageSource: "qrc:///images/go-previous.png"
        anchors.left: parent.left
        anchors.top: parent.top
        height: 64
        width: 64
        z: 2

        onClicked: homeScreen.state = ""
    }

    ListView {
        anchors.fill: parent
        anchors.leftMargin: 64
        model: FolderListModel {
           id: flmodel
           folder: "file:/sdcard/"
           showOnlyReadable: true
           sortField: FolderListModel.Name
           showDotAndDotDot: true
           nameFilters: ["*.odt", "*.ods", "*.odp", "*.doc", "*.xls", "*.ppt", "*.docx", "*.xlsx", "*.pptx"] 
        }
        delegate: Text { 
            text: fileName + (flmodel.isFolder(index) ? "/" : "")
            font.pointSize: 20
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    console.log("Clicked on " + filePath)
                    if (flmodel.isFolder(index)) {
                        flmodel.folder = filePath
                    } else {
                        homeScreen.openDocument(filePath)
                    }
                }            
            }
        }
    }
}
