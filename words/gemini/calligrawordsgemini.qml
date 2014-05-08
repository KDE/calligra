/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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
import org.calligra.CalligraComponents 0.1 as Calligra

Item {
    id: base;
    width: 1280;
    height: 768;
    function openFile(fileName) {
        console.debug("open file: " + fileName);
        Settings.currentFile = fileName;
    }
    Connections {
        target: Settings;
        onCurrentFileChanged: wordsCanvas.source = Settings.currentFile;
    }
    Calligra.TextDocumentCanvas {
        id: wordsCanvas;
        anchors.fill: parent;
        onDocumentModelChanged: {
            console.debug("doc and part: " + doc() + " " + part());
            mainWindow.setDocAndPart(doc(), part());
        }
    }
    Flickable {
        id: controllerFlickable;
        anchors.fill: parent;
        Calligra.CanvasControllerItem {
            canvas: wordsCanvas;
            flickable: controllerFlickable;
        }
    }
    Rectangle {
        anchors {
            top: parent.top;
            right: parent.right;
        }
        height: 64;
        width: 64;
        color: "blue";
        MouseArea {
            anchors.fill: parent;
            onClicked: switchToDesktopAction.trigger();
        }
    }
}
