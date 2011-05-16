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

import QtQuick 1.0
import CalligraMobile 1.0

ListView {
    id: recentFilesListView
    property int buttonWidth
    property int buttonHeight

    width: parent.width; height: parent.height;
    spacing: 10

    model: recentFilesModel
    delegate: Button {
        textPosition: "right"

        text: modelData.name
        width: buttonWidth; height: buttonHeight;
        imageSource: {
            switch(modelData.type) {
                case CADocumentInfo.TextDocument:
                    "qrc:///images/words.png"
                    break;
                case CADocumentInfo.Spreadsheet:
                    "qrc:///images/tables.png"
                    break;
                case CADocumentInfo.Presentation:
                    "qrc:///images/stage.png"
                    break;
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: homeScreen.openDocument(modelData.path);
        }
    }
}

