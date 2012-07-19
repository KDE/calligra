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
import "components"

Page {
    DropShadow {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        height: Constants.GridHeight;
        z: 10;

        Header {
            text: "Share";

            leftArea: Button {
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                highlightColor: Constants.Theme.HighlightColor;
                text: "<";
                onClicked: pageStack.pop();
            }

            rightArea: Button {
                width: Constants.GridWidth * 2;
                height: Constants.GridHeight;
                color: Constants.Theme.TertiaryColor;
                textColor: "white";
                text: "Upload";
                onClicked: pageStack.pop();
            }
        }
    }

    CategorySwitcher {
        anchors.bottom: parent.bottom;
        height: Constants.GridHeight * 7;

        categories: [ { name: "DeviantArt", page: deviantArtPage }, { name: "MediaGoblin", page: mediaGoblinPage } ];

        Component { id: deviantArtPage; Page { Label { anchors.centerIn: parent; text: "DeviantArt" } } }
        Component { id: mediaGoblinPage; Page { Label { anchors.centerIn: parent; text: "MediaGoblin" } } }
    }
}