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
import ":/components"

Panel {
    name: "Presets";

    actions: [
        Button {
            width: Constants.GridWidth;
            height: Constants.GridHeight;

            text: "Add";
            color: "transparent";
            textColor: "white";
            shadow: false;
        },
        Button {
            width: Constants.GridWidth;
            height: Constants.GridHeight;

            text: "Edit";
            color: "transparent";
            textColor: "white";
            shadow: false;
        }
    ]

    peekContents: GridView {
        anchors.fill: parent;

        model: model;
        delegate: delegate;

        cellWidth: Constants.GridWidth;
        cellHeight: Constants.GridHeight;
    }

    fullContents: GridView {
        anchors.fill: parent;

        model: model;
        delegate: delegate;

        cellWidth: Constants.GridWidth;
        cellHeight: Constants.GridHeight;
    }

    ListModel {
        id: model;

        ListElement { name: "Paint"; }
        ListElement { name: "Fill"; }
        ListElement { name: "Smudge"; }
    }

    Component {
        id: delegate;

        Button {
            width: Constants.GridWidth;
            height: Constants.GridHeight;

            checked: GridView.isCurrentItem;

            text: model.name;

            highlightColor: Constants.Theme.HighlightColor;

            onClicked: GridView.view.currentIndex = index;
        }
    }
}