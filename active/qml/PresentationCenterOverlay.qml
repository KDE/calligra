/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

import QtQuick 1.1
import org.kde.plasma.core 0.1 as PlasmaCore

Item {
    id: root
    property QtObject documentController
    clip: true

    PlasmaCore.FrameSvgItem {
        id: controlBarFrame
        imagePath: "widgets/background"
        enabledBorders: "TopBorder|LeftBorder|RightBorder|BottomBorder"
        anchors.fill: parent

        GridView {
            id: grid
            clip: true
            anchors { fill: parent; margins: 10 }
            cellWidth: 296; cellHeight: 216
            model: documentController.documentHandler.paDocumentModel()
            delegate: Item {
                width: GridView.view.cellWidth; height: GridView.view.cellHeight
                Image {
                    anchors { fill: parent; margins: 10 }
                    source: thumbnail

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            documentController.documentHandler.currentSlideNumber = index
                            root.parent.parent.toggleOverlay()
                        }
                    }
                }
            }
        }
    }
}
