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
import KritaTouch.Extensions.AdvancedColorSelectors 1.0

Rectangle
{
    id: mainWindow
    function toggle_colorSelector()
    {
        if(mainWindow.state == "toggle-color-selector-view")
            mainWindow.state = "default"
        else
            mainWindow.state = "toggle-color-selector-view"
    }
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
        MouseArea {
            anchors.fill: parent        
            onClicked: mainWindow.state = "toggle-widgets-view"
        }
    }
    Rectangle {
        id: selectBrush
        color: "blue"
        width: 50
        height: 50
        anchors.top: parent.top
        anchors.right: showWidgets.left

        MouseArea {
            anchors.fill: parent        
            onClicked: mainWindow.state = "toggle-brush-selector-view"
        }
    }
    Rectangle {
        id: colorSelectorButton
        color: "green"
        width: 50
        height: 50
        anchors.top: parent.top
        anchors.right: selectBrush.left
        MouseArea {
            anchors.fill: parent        
            onClicked: toggle_colorSelector()
        }
    }
    ColorSelector {
        id: colorSelector
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: colorSelectorButton.bottom
        visible: false
    }
    states : [
        State {
            name: "default";
        },
        State {
            name: "toggle-widgets-view"
        },
        State {
            name: "toggle-color-selector-view"
            PropertyChanges { target: colorSelector; visible: true }
            
        }
    ]
}
