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
    function toggle_brushSelector()
    {
        if(mainWindow.state == "toggle-brush-selector-view")
            mainWindow.state = "default"
        else
            mainWindow.state = "toggle-brush-selector-view"
    }
    Canvas {
        id: canvas
        objectName: "canvas"
        anchors.fill: parent
    }
    Rectangle {
        id: showWidgetsButton
        color: "red"
        width: 64
        height: 64
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
        width: 64
        height: 64
        anchors.top: parent.top
        anchors.right: showWidgetsButton.left

        MouseArea {
            anchors.fill: parent        
            onClicked: toggle_brushSelector()
        }
    }
    Image {
        id: colorSelectorButton
        source: "image://kicon/format-stroke-color"
        width: 64
        height: 64
        anchors.top: parent.top
        anchors.right: selectBrush.left
        MouseArea {
            anchors.fill: parent        
            onClicked: toggle_colorSelector()
        }
    }
    Rectangle {
        id: widgetAreaBackground
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: showWidgetsButton.bottom
        color: "#BBBBBB"
        opacity: 0.5
        visible: false
    }
    Item {
        id: widgetArea
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: showWidgetsButton.bottom
        visible: false
        
        // Color selector area
        ColorSelector {
            id: colorSelector
            anchors.fill: parent
            resourceManager: canvas.resourceManager
            visible: false
        }
        // Brush selector area
        GridView {
            id: brushSelector
            anchors.fill: parent
            visible: false
            cellWidth: 110
            cellHeight: 120
            model: canvas.brushResourceModel.resourcesInformation
            highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
            delegate: Item
            {
                height: 120
                width: 110
                Icon {
                    x: 5
                    y: 5
                    height: 100
                    width: 100
                    image: model.modelData.image
                }
                Text {
                    x: 5
                    y: 105
                    width: 100
                    elide: Text.ElideRight
                    text: model.modelData.name
                }
                MouseArea {
                    anchors.fill: parent
                    onClicked:
                    {
                        brushSelector.currentIndex = index
                        canvas.brushPreset = model.modelData
                    }
                }
            }
        }
    }
    states : [
        State {
            name: "default";
        },
        State {
            name: "toggle-widgets-view"
            PropertyChanges { target: widgetAreaBackground; visible: true }
            PropertyChanges { target: widgetArea; visible: true }
        },
        State {
            name: "toggle-brush-selector-view"
            PropertyChanges { target: widgetAreaBackground; visible: true }
            PropertyChanges { target: widgetArea; visible: true }
            PropertyChanges { target: brushSelector; visible: true }
        },
        State {
            name: "toggle-color-selector-view"
            PropertyChanges { target: widgetAreaBackground; visible: true }
            PropertyChanges { target: widgetArea; visible: true }
            PropertyChanges { target: colorSelector; visible: true }
        }
    ]
}
