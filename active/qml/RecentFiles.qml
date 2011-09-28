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
import CalligraActive 1.0
import org.kde.plasma.core 0.1 as PlasmaCore

ListView {
    id: recentFilesListView
    property int buttonWidth
    property int buttonHeight
    property string typeFilter

    width: parent.width; height: parent.height;
    spacing: 10

    PlasmaCore.DataSource {
        id: metadataSource
        engine: "org.kde.active.metadata"
        connectedSources: ["ResourcesOfType:" + typeFilter]
        interval: 0
    }
    PlasmaCore.DataModel {
        id: metadataModel
        keyRoleFilter: ".*"
        dataSource: metadataSource
    }

    model: metadataModel
    delegate: Button {
        textPosition: "right"

        text: label
        width: buttonWidth; height: buttonHeight;
        imageSource: {
            switch(typeFilter) {
                case "PaginatedTextDocument":
                    "qrc:///images/words.png"
                    break;
                case "Spreadsheet":
                    "qrc:///images/tables.png"
                    break;
                case "Presentation":
                    "qrc:///images/stage.png"
                    break;
            }
        }

        MouseArea {
            anchors.fill: parent
            onClicked: homeScreen.openDocument(model["url"]);
        }
    }
}

