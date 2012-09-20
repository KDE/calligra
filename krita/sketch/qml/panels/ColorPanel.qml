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
    id: base;
    name: "Color";
    panelColor: "#d37300"
    dragDelegate: Component {
        Rectangle {
            color: "#d37300";
            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
            radius: 8

            Label {
                anchors.centerIn: parent;

                text: "Color";
                color: "white";

                font.pixelSize: Constants.DefaultFontSize;
            }
        }
    }

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
        cellWidth: width / 2;
        cellHeight: Constants.GridHeight;
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
                id: colorSelectorActual;
                anchors.fill: parent;
                view: sketchView.view;
                changeBackground: swatch.chooseBG;
                onColorChanged: {
                    fullPaletteAlphaSlider.value = newAlpha * 100;
                    if(backgroundChanged) {
                        swatch.bgColor = newColor;
                    }
                    else {
                        swatch.fgColor = newColor;
                    }
                }
            }
        }
        Slider {
            id: fullPaletteAlphaSlider;
            anchors {
                top: colorSelector.bottom;
                left: parent.left;
                right: parent.right;
                leftMargin: Constants.DefaultMargin;
                rightMargin: Constants.DefaultMargin;
            }
            value: 100;
            onValueChanged: colorSelectorActual.setAlpha(value);
        }
        ExpandingListView {
            id: fullPaletteList
            anchors {
                top: fullPaletteAlphaSlider.bottom;
                topMargin: Constants.DefaultMargin;
                left: parent.left;
                right: parent.right;
                leftMargin: Constants.DefaultMargin;
                rightMargin: Constants.DefaultMargin;
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
                topMargin: Constants.DefaultMargin;
                left: parent.left;
                right: parent.right;
                rightMargin: Constants.DefaultMargin;
                bottom: parent.bottom;
            }
            model: paletteColorsModel;
            delegate: delegate;
            clip: true;
            cellWidth: width / 2;
            cellHeight: Constants.GridHeight;
        }
    }

    Component {
        id: delegate;

        Item {
            width: Constants.GridWidth;
            height: Constants.GridHeight;
            Image {
                anchors {
                    fill: parent;
                    margins: Constants.DefaultMargin;
                }
                source: model.image;
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        //Settings.currentColor = model.color;
                        paletteColorsModel.activateColor(index, swatch.chooseBG);
                    }
                }
            }
        }
    }
}
