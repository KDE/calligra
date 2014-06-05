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
    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
    anchors {
        top: parent.bottom;
        right: parent.right;
        topMargin: 4;
    }
    height: Constants.GridHeight * 3;
    width: Constants.GridWidth * 2;
    Rectangle {
        anchors.fill: parent;
        color: "white";
        border.color: "#e8e9ea";
        border.width: 1;
        opacity: 0.96;
    }
    Rectangle {
        id: titleBar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.SmallFontSize + 2 * Constants.DefaultMargin;
        color: "#e8e9ea";
        Label {
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }
            text: "IMAGE TOOLS";
            color: "#5b6573";
            font.pixelSize: Constants.SmallFontSize
            font.bold: true;
        }
    }
    Column {
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
                source: Settings.theme.icon("gradient-black");
                sourceSize.width: width > height ? height : width;
                sourceSize.height: width > height ? height : width;
            }
            Slider {
                id: brightnessSlider;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: brightnessLabel.right;
                    right: parent.right;
                    margins: Constants.DefaultMargin;
                }
                highPrecision: true;
                value: 50;
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
                source: Settings.theme.icon("paint-black");
                sourceSize.width: width > height ? height : width;
                sourceSize.height: width > height ? height : width;
            }
            Slider {
                id: contrastSlider;
                anchors {
                    verticalCenter: parent.verticalCenter;
                    left: contrastLabel.right;
                    right: parent.right;
                    margins: Constants.DefaultMargin;
                }
                highPrecision: true;
                value: 50;
            }
        }
    }
}