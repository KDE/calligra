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

ListView {
    id: base;

    signal clicked();

    delegate: Rectangle {
        width: Constants.GridWidth * 4;
        height: Constants.GridHeight;

        Button {
            id: thumbnail;

            image: model.image;

            onClicked: base.clicked();
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
        ListElement { name: "Blank Image (A4)"; image: "../images/document-new.png" }
        ListElement { name: "Blank Image (A3)"; image: "../images/document-new.png" }
        ListElement { name: "Custom Size"; image: "../images/document-new.png" }
        ListElement { name: "From Clipboard"; image: "../images/edit-paste.png" }
        ListElement { name: "From Camera"; image: "../images/camera-web.png" }
    }
}