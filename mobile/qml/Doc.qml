/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>
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

import QtQuick 1.0 as QML
import CalligraMobile 1.0

QML.Rectangle {
    id: rootRect

    width: 800; height: 1600;
    gradient: QML.Gradient {
         QML.GradientStop { position: 0.0; color: "#808080" }
         QML.GradientStop { position: 1.0; color: "#303030" }
    }

    QML.Row {
        anchors.fill: parent

        CanvasController {
            id: canvas

            height: parent.height
            width: parent.width*0.9
        }

        WordsToolbar {
            id: toolbar

            height: parent.height
            width: parent.width*0.1
        }
    }

    QML.Component.onCompleted: canvas.openDocument("/media/Data/Other/all/Documents/Resume.odt");
}
