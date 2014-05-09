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

import QtQuick 1.1
import org.calligra 1.0
import "components"

Page {
    DocumentListModel { id: textDocumentsModel; filter: DocumentListModel.TextDocumentType; }
    ListView {
        id: docList;
        anchors.fill: parent;
        anchors.margins: 5;
        model: textDocumentsModel;
        delegate: documentTile;
    }
    ScrollDecorator { flickableItem: docList; }
    Component { id: mainPage; MainPage { } }
    Component {
        id: documentTile;
        Rectangle {
            width: parent.width;
            height: model.fileName ? 55 : 0;
            color: index % 2 === 1 ? "white" : "silver";
            radius: height / 2;
            Label {
                id: lblName;
                anchors.margins: 5;
                anchors.leftMargin: parent.height / 3;
                anchors.left: parent.left;
                anchors.right: parent.horizontalCenter;
                anchors.verticalCenter: parent.verticalCenter;
                text: model.fileName ? model.fileName : "";
            }
            Label {
                id: lblMTime;
                anchors.margins: 5;
                anchors.left: lblName.right;
                anchors.right: lblSize.left;
                anchors.verticalCenter: parent.verticalCenter;
                text: model.modifiedTime ? model.modifiedTime : "";
                horizontalAlignment: Text.AlignRight;
            }
            Label {
                id: lblSize;
                anchors.margins: 5;
                anchors.rightMargin: parent.height / 3;
                anchors.right: parent.right;
                anchors.verticalCenter: parent.verticalCenter;
                width: parent.width / 5;
                text: model.fileSize ? model.fileSize : "";
                horizontalAlignment: Text.AlignRight;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    openFile(model.filePath);
                }
            }
        }
    }
}
