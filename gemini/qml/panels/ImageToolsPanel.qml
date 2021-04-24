/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
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
    BorderImage {
        anchors {
            fill: parent;
            topMargin: -28;
            leftMargin: -36;
            rightMargin: -36;
            bottomMargin: -44;
        }
        border { left: 36; top: 28; right: 36; bottom: 44; }
        horizontalTileMode: BorderImage.Stretch;
        verticalTileMode: BorderImage.Stretch;
        source: Settings.theme.image("drop-shadows.png");
        BorderImage {
            anchors {
                fill: parent;
                topMargin: 28;
                leftMargin: 36;
                rightMargin: 36;
                bottomMargin: 44;
            }
            border { left: 8; top: 8; right: 8; bottom: 8; }
            horizontalTileMode: BorderImage.Stretch;
            verticalTileMode: BorderImage.Stretch;
            source: Settings.theme.image("drop-corners.png");
        }
    }
    height: Constants.GridHeight * 4 + 2 * Constants.DefaultMargin;
    width: Constants.GridWidth * 3;
    Item {
        id: titleBar;
        clip: true;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight;
        Item {
            anchors.fill: parent;
            clip: true;
            Rectangle {
                anchors {
                    fill: parent;
                    bottomMargin: -(Constants.DefaultMargin + 1);
                    rightMargin: 1;
                }
                radius: Constants.DefaultMargin;
                color: "#22282f";
                opacity: 0.96
                border.width: 1;
                border.color: "#22282f";
            }
        }
        Label {
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin * 2;
                verticalCenter: parent.verticalCenter;
            }
            text: "IMAGE TOOLS";
            color: "#c1cdd1";
            font: Settings.theme.font("applicationSemi");
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
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    anchors.top: parent.bottom;
                    height: Constants.DefaultMargin * 2;
                    width: height;
                    color: "#4e5359";
                    rotation: 45;
                    opacity: parent.checked ? 0.96 : 0;
                    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                }
            }
        }
    }
    Item {
        anchors.fill: contentsColumn;
        clip: true;
        Rectangle {
            anchors {
                fill: parent;
                topMargin: -(Constants.DefaultMargin + 1);
                rightMargin: 1;
            }
            radius: Constants.DefaultMargin;
            color: "#4e5359";
            border.color: "#22282f";
            border.width: 1;
            opacity: 0.96;
        }
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
                height: Constants.GridHeight - Constants.DefaultMargin * 4;
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
                font: Settings.theme.font("toolbar");
                text: brightnessSlider.value - 50;
            }
        }
        Rectangle { height: 1; width: parent.width; color: "black"; opacity: 0.5; }
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
                height: Constants.GridHeight - Constants.DefaultMargin * 4;
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
                font: Settings.theme.font("toolbar");
                text: contrastSlider.value - 50;
            }
        }
        Rectangle { height: 1; width: parent.width; color: "black"; opacity: 0.5; }
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
                height: Constants.GridHeight - Constants.DefaultMargin * 4;
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
                font: Settings.theme.font("toolbar");
                text: transparencySlider.value + "%";
            }
        }
    }
}
