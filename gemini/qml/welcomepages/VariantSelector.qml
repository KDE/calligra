/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0
import "../components"

Item {
    id: base;
    property alias model: variantView.model;
    property string selectorType: "stage";
    anchors.fill: parent;
    enabled: (visible && opacity > 0.0);
    opacity: 0;
    Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
    MouseArea { anchors.fill: parent; onClicked: {} }
    SimpleTouchArea { anchors.fill: parent; onTouched: {} }
    Rectangle {
        anchors.fill: parent;
        color: "#e8e9ea"
        opacity: 0.6;
    }
    Item {
        anchors {
            centerIn: parent;
            topMargin: -(height/2);
        }
        height: Settings.theme.adjustedPixel(475);
        width: Settings.theme.adjustedPixel(1600);
        Rectangle {
            anchors.fill: parent;
            color: "#22282f";
            opacity: 0.7;
            radius: 8;
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
        Label {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: Settings.theme.adjustedPixel(120);
            horizontalAlignment: Text.AlignHCenter;
            verticalAlignment: Text.AlignVCenter;
            color: "white";
            font: Settings.theme.font("title");
            text: "Select a Style Option";
        }
        Button {
            anchors {
                top: parent.top;
                right: parent.right;
                margins: Settings.theme.adjustedPixel(20);
            }
            height: Settings.theme.adjustedPixel(40);
            width: height;
            image: Settings.theme.icon("SVG-Icon-SmallX");
            onClicked: base.opacity = 0;
        }
        Flickable {
            id: variantFlickable;
            anchors {
                fill: parent;
                topMargin: Settings.theme.adjustedPixel(120);
                leftMargin: Settings.theme.adjustedPixel(50);
                rightMargin: Settings.theme.adjustedPixel(50);
                bottomMargin: Settings.theme.adjustedPixel(40);
            }
            clip: true;
            contentHeight: variantFlow.height;
            contentWidth: variantFlow.width;
            Flow {
                id: variantFlow;
                width: variantFlickable.width;
                Repeater {
                    id: variantView;
                    delegate: Item {
                        height: Settings.theme.adjustedPixel(310);
                        width: Settings.theme.adjustedPixel(375);
                        MouseArea {
                            anchors.fill: parent;
                            onClicked: {
                                base.opacity = 0;
                                var file = "";
                                if(base.selectorType === "stage" || base.selectorType === "words") {
                                    file = "template://" + model.url;
                                }
                                if(file.slice(-1) === "/" || file === "") {
                                    return;
                                }
                                openFile(file);
                            }
                            Rectangle {
                                anchors.fill: parent;
                                opacity: parent.pressed ? 0.6 : 0;
                                Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                                radius: Settings.theme.adjustedPixel(8);
                                color: "#00adf5";
                            }
                        }
                        Image {
                            anchors {
                                top: parent.top;
                                left: parent.left;
                                right: parent.right;
                                margins: Settings.theme.adjustedPixel(16);
                            }
                            height: Settings.theme.adjustedPixel(192);
                            source: model.thumbnail;
                            fillMode: Image.PreserveAspectFit
                            smooth: true;
                        }
                        Image {
                            anchors {
                                horizontalCenter: parent.horizontalCenter;
                                bottom: parent.bottom;
                                margins: Settings.theme.adjustedPixel(16);
                            }
                            height: Settings.theme.adjustedPixel(64);
                            width: Settings.theme.adjustedPixel(190);
                            source: model.swatch !== "" ? Settings.theme.icon(model.swatch) : "";
                            fillMode: Image.PreserveAspectFit
                            smooth: true;
                        }
                        Label {
                            anchors {
                                horizontalCenter: parent.horizontalCenter;
                                bottom: parent.bottom;
                                margins: Settings.theme.adjustedPixel(16);
                            }
                            height: Settings.theme.adjustedPixel(64);
                            width: Settings.theme.adjustedPixel(190);
                            horizontalAlignment: Text.AlignHCenter;
                            text: model.text;
                            font: Settings.theme.font("templateLabel");
                            color: "white";
                        }
                    }
                }
            }
        }
        ScrollDecorator { flickableItem: variantFlickable; anchors.fill: variantFlickable; }
    }
}
