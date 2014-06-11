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
import "../components"

Page {
    id: base;
    DocumentListModel { id: textDocumentsModel; filter: DocumentListModel.TextDocumentType; }
    DocumentListModel { id: presentationDocumentsModel; filter: DocumentListModel.PresentationType; }
    property string categoryUIName: (docList.model === textDocumentsModel) ? "text documents" : "presentations"
    Row {
        id: docTypeSelectorRow;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        height: Constants.GridHeight;
        spacing: Constants.DefaultMargin;
        Button {
            width: parent.width / 2;
            height: Constants.GridHeight - Constants.DefaultMargin * 2;
            text: "Text Documents";
            color: Settings.theme.color("base/base");
            textColor: Settings.theme.color("base/text");
            onClicked: docList.model = textDocumentsModel;
            checked: docList.model === textDocumentsModel;
        }
        Button {
            width: parent.width / 2;
            height: Constants.GridHeight - Constants.DefaultMargin * 2;
            text: "Presentations";
            color: Settings.theme.color("base/base");
            textColor: Settings.theme.color("base/text");
            onClicked: docList.model = presentationDocumentsModel;
            checked: docList.model === presentationDocumentsModel;
        }
    }
    ListView {
        id: docList;
        clip: true;
        anchors {
            margins: Constants.DefaultMargin;
            top: docTypeSelectorRow.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        model: textDocumentsModel;
        delegate: documentTile;
        ScrollDecorator { flickableItem: docList; }
    }
    Label {
        anchors.centerIn: parent;
        text: "No %1 - please drop some into your Documents folder (%2)".arg(base.categoryUIName).arg(docList.model.documentsFolder);
        visible: docList.count === 0;
    }
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
                    baseLoadingDialog.visible = true;
                    openFile(model.filePath);
                }
            }
        }
    }
}
