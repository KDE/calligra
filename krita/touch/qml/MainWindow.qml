/*
 * Copyright (C) 2012 Cyrille Berger <cberger@cberger.net>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 sure
 * USA
 */

import QtQuick 1.0
import KritaTouch 1.0

Rectangle
{
    id: mainWindow
    Canvas {
        id: canvas
        objectName: "canvas"
        anchors.fill: parent
    }
    Rectangle {
        id: showWidgets
        color: "red"
        width: 50
        height: 50
        anchors.top: parent.top
        anchors.right: parent.right
    }
    Rectangle {
        id: selectBrush
        color: "blue"
        width: 50
        height: 50
        anchors.top: parent.top
        anchors.right: showWidgets.left
    }
    Rectangle {
        id: colorSelector
        color: "green"
        width: 50
        height: 50
        anchors.top: parent.top
        anchors.right: selectBrush.left
    }
}
