/*
 * Copyright 2012  Yue Liu <yue.liu@mail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

import QtQuick 1.0

Rectangle {
    anchors.fill: parent
    color: "white"

    Component {
        id: shapeDelegate

        Item {
            width: grid.cellWidth; height: grid.cellHeight

            Image {
                id: shapeIcon
                width: 48; height: 48
                y: 0; anchors.horizontalCenter: parent.horizontalCenter
                source: icon
            }
            Text {
                anchors { top: shapeIcon.bottom; horizontalCenter: parent.horizontalCenter }
                text: name
            }
        }
    }

    Component {
        id: shapeHighlight
        Rectangle { width: grid.cellWidth; height: grid.cellHeight; color: "lightsteelblue" }
    }

    GridView {
        id: grid
        anchors.fill: parent
        cellWidth: 64; cellHeight: 64
        header: StencilHeader {}

        highlight: shapeHighlight
        focus: true
        model: appModel
        delegate: shapeDelegate
    }
}
