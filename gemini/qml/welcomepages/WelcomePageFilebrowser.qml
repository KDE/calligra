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
    GridView {
        id: docList;
        clip: true;
        contentWidth: width;
        anchors {
            margins: Constants.DefaultMargin;
            top: docTypeSelectorRow.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
            bottomMargin: 0;
        }
        cellWidth: width / 4 - Constants.DefaultMargin;
        cellHeight: cellWidth + Settings.theme.font("templateLabel").pixelSize + Constants.DefaultMargin * 4;
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
        Item {
            width: docList.cellWidth;
            height: docList.cellHeight
            Image {
                source: "image://recentimage/" + model.filePath;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    margins: Constants.DefaultMargin / 2;
                }
                height: parent.width;
                fillMode: Image.PreserveAspectFit;
            }
            Label {
                id: lblName;
                anchors {
                    left: parent.left;
                    right: parent.right;
                    bottom: parent.bottom;
                    margins: Constants.DefaultMargin;
                    bottomMargin: Constants.DefaultMargin * 2;
                }
                height: font.pixelSize + Constants.DefaultMargin * 2;
                horizontalAlignment: Text.AlignHCenter;
                verticalAlignment: Text.AlignVCenter;
                text: model.fileName ? model.fileName : "";
                font: Settings.theme.font("templateLabel");
                color: "#5b6573";
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
