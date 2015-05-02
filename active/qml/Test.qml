/*
 * This file is part of the KDE project
 * Copyright (C) 2014 Arjen Hiemstra <ahiemstra@heimr.nl>
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
import org.calligra.CalligraComponents 0.1 as Calligra

Flickable {
    Calligra.PresentationCanvas {
        id: canvas
        source: "/home/ahiemstra/Documents/114096.odp"
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
