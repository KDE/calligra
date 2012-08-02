/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

PageStack {
    id: base;
    initialPage: createNewListPage;
    clip: true;

    signal clicked();

    Component { id: createNewListPage; Page {
        ListView {
            anchors.fill: parent;

            delegate: Rectangle {
                width: parent.width;
                height: Constants.GridHeight;

                MouseArea {
                    anchors.fill: parent;
                    onClicked: pageStack.push( createNewPage );
                }

                Button {
                    id: thumbnail;

                    image: model.image;

                    onClicked: pageStack.push( createNewPage );
                }

                Label {
                    anchors {
                        left: thumbnail.right;
                        leftMargin: Constants.DefaultMargin;
                        verticalCenter: parent.verticalCenter;
                    }

                    text: model.name;
                }
            }

            model: ListModel {
                ListElement { name: "Blank Image (A4)"; image: "image://icon/document-new" }
                ListElement { name: "Blank Image (A3)"; image: "image://icon/document-new" }
                ListElement { name: "Custom Size"; image: "image://icon/document-new" }
                ListElement { name: "From Clipboard"; image: "image://icon/edit-paste" }
                ListElement { name: "From Camera"; image: "image://icon/camera-web" }
            }
        }
    } }

    Component { id: createNewPage; Page {
        Column {
            anchors.fill: parent;
            Item {
                width: parent.width;
                height: Constants.GridHeight;
                Image {
                    id: titleImage;
                    anchors.left: parent.left;
                    anchors.verticalCenter: parent.verticalCenter;
                    anchors.margins: Constants.DefaultMargin;
                    source: "image://icon/document-new"
                }
                Label {
                    anchors.left: titleImage.right;
                    anchors.margins: Constants.DefaultMargin;
                    anchors.verticalCenter: parent.verticalCenter;
                    text: "Custom Size";
                    font.pixelSize: Constants.LargeFontSize;
                }
            }
            TextField { placeholder: "Width" }
            TextField { placeholder: "Height" }
            TextField { placeholder: "Resolution" }
            Item { width: parent.width; height: Constants.GridHeight; }
            Row {
                width: parent.width;
                Button {
                    width: parent.width / 2;
                    color: Constants.Theme.NegativeColor;
                    text: "Cancel";
                    textColor: "white";
                    onClicked: pageStack.pop();
                }
                Button {
                    width: parent.width / 2;
                    color: Constants.Theme.PositiveColor;
                    text: "Create";
                    textColor: "white";
                    onClicked: base.clicked();
                }
            }
        }
    } }
}
