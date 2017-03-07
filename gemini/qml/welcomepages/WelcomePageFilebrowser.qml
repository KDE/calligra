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
import org.calligra 1.0
import "../components"

Page {
    id: base;
    property string categoryUIName: (docList.model === textDocumentsModel) ? "text documents" : "presentations"
    Label {
        id: docTypeSelectorRow;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight * 1.5;
        verticalAlignment: Text.AlignVCenter;
        horizontalAlignment: Text.AlignHCenter;
        font: Settings.theme.font("pageHeader");
        text: "Open From Your Library";
        color: "#22282f";
        CohereButton {
            anchors {
                left: parent.left;
                leftMargin: 20;
                verticalCenter: parent.verticalCenter;
            }
            text: "Open Other...";
            textColor: "#5b6573";
            textSize: Settings.theme.adjustedPixel(18);
            color: "#D2D4D5";
            onClicked: mainWindow.openFile();
        }
        Row {
            anchors {
                right: parent.right;
                rightMargin: 20;
                verticalCenter: parent.verticalCenter;
            }
            height: parent.height - Constants.DefaultMargin * 2;
            spacing: 4;
            CohereButton {
                anchors.verticalCenter: parent.verticalCenter;
                text: "Text Documents";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                checkedColor: "#D2D4D5";
                onClicked: { if(!checked) { docList.model = textDocumentsModel; } }
                checked: docList.model === textDocumentsModel;
            }
            CohereButton {
                anchors.verticalCenter: parent.verticalCenter;
                text: "Presentations";
                textColor: "#5b6573";
                textSize: Settings.theme.adjustedPixel(18);
                checkedColor: "#D2D4D5";
                onClicked: { if(!checked) { docList.model = presentationDocumentsModel; } }
                checked: docList.model === presentationDocumentsModel;
            }
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
        anchors.fill: parent;
        text: "No %1\n\nPlease drop some into your Documents folder\n(%2)".arg(base.categoryUIName).arg(docList.model.documentsFolder);
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        font: Settings.theme.font("templateLabel");
        color: "#5b6573";
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
                smooth: true;
                asynchronous: true;
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
