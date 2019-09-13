/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import QtQuick 2.0
import QtQuick.Controls 2.2 as QtControls
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0
import "../components"

Kirigami.ScrollablePage {
    id: base;
    objectName: "WelcomePageFilebrowser";
    title: "Open From Your Library";
    property string categoryUIName: (docList.model === textDocumentsModel) ? "text documents" : "presentations"
    actions {
        main: Kirigami.Action {
            text: "Open Other...";
            icon.name: "document-open";
            onTriggered: mainWindow.openFile();
        }
        contextualActions: [
            Kirigami.Action {
                text: "Text Documents";
                onTriggered: { if(!checked) { docList.model = textDocumentsModel; } }
                checked: docList.model === textDocumentsModel;
            },
            Kirigami.Action {
                text: "Presentations";
                onTriggered: { if(!checked) { docList.model = presentationDocumentsModel; } }
                checked: docList.model === presentationDocumentsModel;
            }
        ]
    }
    GridView {
        id: docList;
        contentWidth: width;
        cellWidth: width / 4 - Kirigami.Units.largeSpacing;
        cellHeight: cellWidth + Settings.theme.font("templateLabel").pixelSize + Kirigami.Units.largeSpacing * 4;
        model: textDocumentsModel;
        delegate: documentTile;
        QtControls.Label {
            anchors.fill: parent;
            text: "No %1\n\nPlease drop some into your Documents folder\n(%2)".arg(base.categoryUIName).arg(docList.model.documentsFolder);
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
            visible: docList.count === 0;
        }
    }

    Component {
        id: documentTile;
        DocumentTile {
            width: docList.cellWidth;
            height: docList.cellHeight
            title: model.fileName != "" ? model.fileName : ""
            filePath: model.filePath
            onClicked: openFile(model.filePath);
        }
    }
}
