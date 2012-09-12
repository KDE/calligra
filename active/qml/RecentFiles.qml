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

import QtQuick 1.0
import CalligraActive 1.0
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.components 0.1 as PlasmaComponents

ListView {
    id: recentFilesListView
    property int buttonWidth
    property int buttonHeight
    property string typeFilter

    width: parent.width; height: parent.height;
    spacing: 10

    PlasmaCore.SortFilterModel {
        id: metadataModel
        sourceModel: metadataInternalModel
        sortOrder: Qt.AscendingOrder
    }

    model: metadataModel
    delegate:
        PlasmaComponents.Button {
            text: label
            width: buttonWidth; height: buttonHeight;
            iconSource: {
                switch(typeFilter) {
                    case "PaginatedTextDocument":
                        "words"
                        break;
                    case "Spreadsheet":
                        "kspread"
                        break;
                    case "Presentation":
                        "stage"
                        break;
                }
            }

            onClicked: homeScreen.openDocument(model["url"]);
        }

    onTypeFilterChanged: {
        metadataInternalModel.resourceType = "nfo:" + typeFilter;
        if (typeFilter == "PaginatedTextDocument") {
            metadataModel.filterRole = "mimeType";
            metadataModel.filterRegExp = "application/vnd.oasis.opendocument.text";
        } else {
            metadataModel.filterRole = "";
            metadataModel.filterRegExp = "";
        }
    }

    PlasmaComponents.Label {
        text: "No files here"
        anchors.centerIn: parent
        visible: recentFilesListView.count == 0 && typeFilter != ""
    }
}

