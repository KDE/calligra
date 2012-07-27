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

Item {
    id: base;
    signal clicked();

    ListView {
        anchors.left: parent.left;
        anchors.right: parent.right;
        anchors.top: parent.top;

        height: Constants.GridHeight * 5;

        clip: true;

        delegate: Rectangle {
            width: Constants.GridWidth * 4;
            height: Constants.GridHeight;

            MouseArea {
                anchors.fill: parent;
                onClicked: base.clicked();
            }

            Button {
                id: thumbnail;
                onClicked: base.clicked();
                image: model.image;
            }

            Label {
                anchors {
                    top: parent.top;
                    topMargin: Constants.DefaultMargin;
                    left: thumbnail.right;
                    leftMargin: Constants.DefaultMargin;
                }

                text: model.name;
                verticalAlignment: Text.AlignTop;
            }

            Label {
                anchors {
                    bottom: parent.bottom;
                    bottomMargin: Constants.DefaultMargin;
                    left: thumbnail.right;
                    leftMargin: Constants.DefaultMargin;
                }

                text: model.date;
                color: Constants.Theme.SecondaryTextColor;
                font.pixelSize: Constants.SmallFontSize;
                verticalAlignment: Text.AlignBottom;
            }
        }

        model: ListModel {
            ListElement { name: "Recent Image 00"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
            ListElement { name: "Recent Image 01"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
            ListElement { name: "Recent Image 02"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
            ListElement { name: "Recent Image 03"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
            ListElement { name: "Recent Image 04"; date: "18-07-2012 18:00"; image: "../images/image-x-generic.png" }
        }
    }

    Item {
        anchors.bottom: parent.bottom;
        width: parent.width;
        height: Constants.GridHeight;

        MouseArea {
            anchors.fill: parent;
            onClicked: base.clicked();
        }

        Button {
            id: icon;
            onClicked: base.clicked();
            image: "../images/document-open.png";
        }

        Label {
            anchors {
                left: icon.right;
                leftMargin: Constants.DefaultMargin;
            }

            text: "Open Image";
        }
    }
}