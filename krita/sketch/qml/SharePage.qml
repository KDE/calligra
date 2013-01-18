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
import org.krita.sketch 1.0
import "components"
import "panels/sharepages"

Page {

    Rectangle {
        anchors.fill: parent;
    }

    Header {
        id: header;

        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }

        text: "Share";

        background: "images/header_red.png";
        leftArea: Button {
            width: Constants.GridWidth;
            height: Constants.GridHeight;
            highlightColor: Constants.Theme.HighlightColor;
            image: "./images/svg/icon-back.svg";
            onClicked: pageStack.pop();
        }

        rightArea: Button {
            width: Constants.GridWidth * 2;
            height: Constants.GridHeight;
            color: Constants.Theme.PositiveColor;
            textColor: "white";
            text: "Upload";
            onClicked: {
                switcher.currentPage.submitArt(sharingManager.sharingHandler(switcher.currentPage.pluginName));
            }
        }
    }

    Image {
        anchors.top: header.bottom;
        anchors.left: parent.left;
        anchors.right: parent.right;
        source: "images/shadow-smooth.png";
        z: 5;
    }

    SharingManager {
        id: sharingManager;
        view: sketchView.view;
    }

    CategorySwitcher {
        id: switcher;
        anchors.bottom: parent.bottom;
        height: Constants.GridHeight * (Constants.GridRows - 2);

        categories: [ { name: "DeviantArt Sta.sh", page: contentPage }/*, { name: "MediaGoblin", page: contentPage }*/ ];

        Component {
            id: contentPage;
            DeviantArt {
            }
        }
    }
}
