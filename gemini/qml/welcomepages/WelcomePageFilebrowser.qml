/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.2 as QtControls
import org.kde.kirigami 2.14 as Kirigami
import org.calligra 1.0
import "../components"

Kirigami.ScrollablePage {
    id: base;
    objectName: "WelcomePageFilebrowser";
    title: i18n("Open From Your Library");
    property string categoryUIName: (docList.model === textDocumentsModel) ? "text documents" : "presentations"
    actions: [
        Kirigami.Action {
            text: i18n("Open Other...");
            icon.name: "document-open";
            onTriggered: mainWindow.openFile();
        },
        Kirigami.Action {
            text: i18n("Text Documents");
            onTriggered: {
                if (!checked) {
                    docList.model = textDocumentsModel;
                }
            }
            checked: docList.model === textDocumentsModel;
        },
        Kirigami.Action {
            text: i18n("Presentations");
            onTriggered: {
                if(!checked) {
                    docList.model = presentationDocumentsModel;
                }
            }
            checked: docList.model === presentationDocumentsModel;
        }
    ]

    GridView {
        id: docList;
        cellWidth: Math.floor(width/Math.floor(width/(Kirigami.Units.gridUnit * 8 + Kirigami.Units.largeSpacing * 2)))
        cellHeight: cellWidth + Kirigami.Theme.defaultFont.pixelSize
        model: textDocumentsModel;
        delegate: DocumentTile {
            title: model.fileName.length > 0 ? model.fileName : ""
            filePath: model.filePath
            onClicked: openFile(model.filePath);
        }
        Kirigami.PlaceholderMessage {
            anchors.centerIn: parent;
            text: i18n("No %1\n\nPlease drop some into your Documents folder\n(%2)", base.categoryUIName, docList.model.documentsFolder);
            width: parent.width - (Kirigami.Units.largeSpacing * 4)
            visible: docList.count === 0;
        }
    }
}
