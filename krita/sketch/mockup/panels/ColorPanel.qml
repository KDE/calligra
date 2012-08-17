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
import "../components"

Panel {
    name: "Color";
    panelColor: "#d37300"

    peekContents: GridView {
        anchors.fill: parent;
        model: PaletteColorsModel
        delegate: delegate;
        cellWidth: Constants.GridWidth;
        cellHeight: Constants.GridHeight;
    }

    fullContents: Item {
        anchors.fill: parent;
        ListView {
            id: fullPaletteList
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: Constants.GridHeight;
            orientation: ListView.Horizontal;
            model: PaletteModel
            delegate: Button {
                height: Constants.GridHeight;
                width: Constants.GridWidth;
                text: model.text;
                textSize: 10;
                image: model.image;
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: {
                    ListView.view.currentIndex = index;
                    PaletteModel.itemActivated(index);
                    PaletteColorsModel.colorSet = PaletteModel.colorSet;
                }
            }
        }
        GridView {
            anchors {
                top: fullPaletteList.bottom;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            model: PaletteColorsModel;
            delegate: delegate;
            clip: true;
            cellWidth: Constants.GridWidth - 8;
            cellHeight: Constants.GridHeight - 8;
        }
    }

    Component {
        id: delegate;

        Button {
            width: Constants.GridWidth;
            height: Constants.GridHeight;

            checked: GridView.isCurrentItem;

            text: model.text;
            shadow: false
            textSize: 10;
            image: model.image;

            highlightColor: Constants.Theme.HighlightColor;

            onClicked: {
                GridView.view.currentIndex = index;
                //Settings.currentColor = model.color;
            }
        }
    }
}
