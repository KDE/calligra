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
import "../components"

Panel {
    name: "Color";
    panelColor: "#d37300"
    
    actions: [
        ColorSwatch {
            id: swatch;
            height: parent.height;
            width: height;
        }
    ]
    
    PaletteColorsModel {
        id: paletteColorsModel;
        view: sketchView.view;
        onColorChanged: {
            if(backgroundChanged) {
                swatch.bgColor = newColor;
            }
            else {
                swatch.fgColor = newColor;
            }
        }
    }
    PaletteModel {
        id: paletteModel;
    }

    peekContents: GridView {
        anchors.fill: parent;
        model: paletteColorsModel;
        delegate: delegate;
        cellWidth: Constants.GridWidth - 8;
        cellHeight: Constants.GridHeight - 8;
    }

    fullContents: Item {
        anchors.fill: parent;
        Item {
            id: colorSelector;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
                margins: Constants.DefaultMargin;
            }
            height: parent.width;
            ColorSelectorItem {
                anchors.fill: parent;
                view: sketchView.view;
                onColorChanged: {
                    if(backgroundChanged) {
                        swatch.bgColor = newColor;
                    }
                    else {
                        swatch.fgColor = newColor;
                    }
                }
            }
        }
        ExpandingListView {
            id: fullPaletteList
            anchors {
                top: colorSelector.bottom;
                left: parent.left;
                right: parent.right;
            }
            model: paletteModel
            onCurrentIndexChanged: {
                paletteModel.itemActivated(currentIndex);
                paletteColorsModel.colorSet = paletteModel.colorSet;
            }
        }
        GridView {
            anchors {
                top: fullPaletteList.bottom;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            model: paletteColorsModel;
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

            color: "transparent";
            text: model.text;
            shadow: false
            textSize: 10;
            image: model.image;

            highlightColor: Constants.Theme.HighlightColor;

            onClicked: {
                GridView.view.currentIndex = index;
                //Settings.currentColor = model.color;
                paletteColorsModel.activateColor(index, swatch.chooseBG);
            }
        }
    }
}
