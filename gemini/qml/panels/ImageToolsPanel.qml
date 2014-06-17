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

import QtQuick 1.1
import "../components"
import org.calligra 1.0

Item {
    opacity: parent.checked ? 1 : 0;
    property QtObject canvas: null;
    Connections {
        target: canvas;
        onShapeTransparencyChanged: transparencySlider.value = 100 - (100 * canvas.shapeTransparency);
    }
    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
    anchors {
        top: parent.bottom;
        right: parent.right;
        topMargin: 4;
    }
    height: Constants.GridHeight * 4 + 2 * Constants.DefaultMargin;
    width: Constants.GridWidth * 3;
    Item {
        id: titleBar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight;
        Rectangle {
            anchors.fill: parent;
            color: "#22282f";
            opacity: 0.96
            border.width: 1;
            border.color: "#22282f";
        }
        Label {
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }
            text: "IMAGE TOOLS";
            color: "#c1cdd1";
            font.pixelSize: Constants.SmallFontSize
            font.bold: true;
        }
        Row {
            anchors {
                right: parent.right;
                rightMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }
            height: parent.height - Constants.DefaultMargin * 2;
            spacing: Constants.DefaultMargin;
            Button {
                image: Settings.theme.icon("SVG-IMGTOOLS-ReplaceImage-1");
                height: parent.height; width: height;
            }
            Button {
                image: Settings.theme.icon("SVG-IMGTOOLS-Wand-1");
                height: parent.height; width: height;
            }
            Button {
                image: Settings.theme.icon("SVG-IMGTOOLS-Crop-1");
                height: parent.height; width: height;
            }
            Button {
                image: Settings.theme.icon("SVG-IMGTOOLS-Sliders-1");
                height: parent.height; width: height;
                checked: true;
                checkedColor: "#00adf5";
                checkedOpacity: 0.6;
                radius: 4;
            }
        }
    }
    Rectangle {
        anchors.fill: contentsColumn;
        color: "#4e5359";
        border.color: "#22282f";
        border.width: 1;
        opacity: 0.96;
    }
    Column {
        id: contentsColumn;
        anchors {
            top: titleBar.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        Item {
            width: parent.width;
            height: Constants.GridHeight;
            Image {
                id: brightnessLabel;
                anchors {
                    left: parent.left;
                    top: parent.top;
                    margins: Constants.DefaultMargin;
                }
                height: parent.height - Constants.DefaultMargin * 2;
                width: height;
                source: Settings.theme.icon("SVG-IMGTOOLS-Brightness-1");
                sourceSize.width: width > height ? height : width;
                sourceSize.height: width > height ? height : width;
            }
            Slider {
                id: brightnessSlider;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: brightnessLabel.right;
                    right: brightnessValue.left;
                    margins: Constants.DefaultMargin;
                }
                Component.onCompleted: value = 50;
            }
            Label {
                id: brightnessValue;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                    margins: Constants.DefaultMargin;
                }
                height: parent.height - Constants.DefaultMargin * 2;
                width: height;
                color: "#c1cdd1";
                text: brightnessSlider.value - 50;
            }
        }
        Item {
            width: parent.width;
            height: Constants.GridHeight;
            Image {
                id: contrastLabel;
                anchors {
                    left: parent.left;
                    top: parent.top;
                    margins: Constants.DefaultMargin;
                }
                height: parent.height - Constants.DefaultMargin * 2;
                width: height;
                source: Settings.theme.icon("SVG-IMGTOOLS-Contrast-1");
                sourceSize.width: width > height ? height : width;
                sourceSize.height: width > height ? height : width;
            }
            Slider {
                id: contrastSlider;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: contrastLabel.right;
                    right: contrastValue.left;
                    margins: Constants.DefaultMargin;
                }
                Component.onCompleted: value = 50;
            }
            Label {
                id: contrastValue;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                    margins: Constants.DefaultMargin;
                }
                height: parent.height - Constants.DefaultMargin * 2;
                width: height;
                color: "#c1cdd1";
                text: contrastSlider.value - 50;
            }
        }
        Item {
            width: parent.width;
            height: Constants.GridHeight;
            Image {
                id: transparencyLabel;
                anchors {
                    left: parent.left;
                    top: parent.top;
                    margins: Constants.DefaultMargin;
                }
                height: parent.height - Constants.DefaultMargin * 2;
                width: height;
                source: Settings.theme.icon("SVG-IMGTOOLS-Transparency-1");
                sourceSize.width: width > height ? height : width;
                sourceSize.height: width > height ? height : width;
            }
            Slider {
                id: transparencySlider;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: transparencyLabel.right;
                    right: transparencyValue.left;
                    margins: Constants.DefaultMargin;
                }
                highPrecision: false;
                Component.onCompleted: value = 100;
                onValueChanged: { if(canvas) { canvas.shapeTransparency = (100 - value) / 100; } }
            }
            Label {
                id: transparencyValue;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    right: parent.right;
                    margins: Constants.DefaultMargin;
                }
                height: parent.height - Constants.DefaultMargin * 2;
                width: height;
                color: "#c1cdd1";
                text: transparencySlider.value + "%";
            }
        }
    }
}