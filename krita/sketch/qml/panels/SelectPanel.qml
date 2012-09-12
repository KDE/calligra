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
    name: "Select";
    panelColor: Constants.Theme.QuaternaryColor;
    
    actions: [
        Button {
            id: deselectButton;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-select-deselect.svg"
            textColor: "white";
            shadow: false;
            highlight: false;
        },
        Button {
            id: reselectButton;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-select-reselect.svg"
            textColor: "white";
            shadow: false;
            highlight: false;
        },
        Item {
            width: (Constants.GridWidth * 2) - Constants.DefaultMargin - (Constants.GridHeight * 3)
            height: Constants.GridHeight;
        },
        Button {
            id: toggleShowSelectionButton;
            property bool showSelection: true;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: showSelection ? "../images/svg/icon-select-show.svg" : "../images/svg/icon-select-hide.svg";
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: showSelection = !showSelection;
        }
    ]
    
    peekContents: Item {
        id: peekItem;
        anchors.fill: parent;
        
        Item {
            width: childrenRect.width;
            height: peekItem.height / 3;
            anchors {
                horizontalCenter: parent.horizontalCenter;
                top: parent.top;
            }
            Button {
                id: selectRectangle;
                anchors.verticalCenter: parent.verticalCenter;
                image: "../images/svg/icon-select-rectangle.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                textColor: "white";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectPolygon;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectRectangle.right;
                image: "../images/svg/icon-select-polygon.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                textColor: "white";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectArea;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectPolygon.right;
                image: "../images/svg/icon-select-area.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                textColor: "white";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectColor;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectArea.right;
                image: "../images/svg/icon-select-color.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                textColor: "white";
                shadow: false;
                highlight: false;
            }
        }
        Item {
            width: childrenRect.width;
            height: childrenRect.height;
            anchors.centerIn: parent;
            Button {
                id: selectReplace;
                image: "../images/svg/icon-select-replace.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectIntersect;
                anchors.left: selectReplace.right;
                image: "../images/svg/icon-select-intersect.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectAdd;
                anchors.left: selectIntersect.right;
                image: "../images/svg/icon-select-add.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectSub;
                anchors.left: selectAdd.right;
                image: "../images/svg/icon-select-sub.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
        }
        Item {
            width: childrenRect.width;
            height: peekItem.height / 3;
            anchors {
                horizontalCenter: parent.horizontalCenter;
                bottom: parent.bottom;
            }
            Button {
                id: selectAll;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.margins: Constants.DefaultMargin;
                text: "All";
                textColor: "black";
                color: "#63ffffff";
                border.width: 1;
                border.color: "silver";
                radius: Constants.DefaultMargin;
                width: (Constants.GridWidth * 2 / 3) - Constants.DefaultMargin;
                height: textSize + Constants.DefaultMargin * 2
            }
            Button {
                id: selectInvert;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectAll.right;
                anchors.margins: Constants.DefaultMargin;
                text: "Invert";
                textColor: "black";
                color: "#63ffffff";
                border.width: 1;
                border.color: "silver";
                radius: Constants.DefaultMargin;
                width: (Constants.GridWidth * 2 / 3) - Constants.DefaultMargin;
                height: textSize + Constants.DefaultMargin * 2
            }
            Button {
                id: selectOpaque;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectInvert.right;
                anchors.margins: Constants.DefaultMargin;
                text: "Opaque";
                textColor: "black";
                color: "#63ffffff";
                border.width: 1;
                border.color: "silver";
                radius: Constants.DefaultMargin;
                width: (Constants.GridWidth * 2 / 3) - Constants.DefaultMargin;
                height: textSize + Constants.DefaultMargin * 2
            }
        }
    }
    
    fullContents: Item {
        id: fullItem;
        anchors.fill: parent;
        
        Item {
            id: fullTopRow;
            width: childrenRect.width;
            height: Constants.GridHeight;
            anchors {
                horizontalCenter: parent.horizontalCenter;
                top: parent.top;
            }
            Button {
                id: selectRectangleFull;
                anchors.verticalCenter: parent.verticalCenter;
                image: "../images/svg/icon-select-rectangle.svg"
                width: (Constants.GridWidth / 2) - Constants.DefaultMargin;
                height: width;
                color: "transparent";
                textColor: "white";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectPolygonFull;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectRectangleFull.right;
                image: "../images/svg/icon-select-polygon.svg"
                width: (Constants.GridWidth / 2) - Constants.DefaultMargin;
                height: width;
                color: "transparent";
                textColor: "white";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectAreaFull;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectPolygonFull.right;
                image: "../images/svg/icon-select-area.svg"
                width: (Constants.GridWidth / 2) - Constants.DefaultMargin;
                height: width;
                color: "transparent";
                textColor: "white";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectColorFull;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectAreaFull.right;
                image: "../images/svg/icon-select-color.svg"
                width: (Constants.GridWidth / 2) - Constants.DefaultMargin;
                height: width;
                color: "transparent";
                textColor: "white";
                shadow: false;
                highlight: false;
            }
        }
        Label {
            id: modeLabel
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
                top: fullTopRow.bottom;
            }
            horizontalAlignment: Text.AlignLeft;
            font.pixelSize: Constants.LargeFontSize;
            font.bold: true;
            height: Constants.GridHeight / 2;
            text: "Mode:";
        }
        Item {
            id: fullModeRow;
            width: childrenRect.width;
            height: Constants.GridHeight;
            anchors.horizontalCenter: parent.horizontalCenter;
            anchors.top: modeLabel.bottom;
            Button {
                id: selectReplaceFull;
                image: "../images/svg/icon-select-replace.svg"
                width: (Constants.GridWidth / 2) - Constants.DefaultMargin;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectIntersectFull;
                anchors.left: selectReplaceFull.right;
                image: "../images/svg/icon-select-intersect.svg"
                width: (Constants.GridWidth / 2) - Constants.DefaultMargin;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectAddFull;
                anchors.left: selectIntersectFull.right;
                image: "../images/svg/icon-select-add.svg"
                width: (Constants.GridWidth / 2) - Constants.DefaultMargin;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
            Button {
                id: selectSubFull;
                anchors.left: selectAddFull.right;
                image: "../images/svg/icon-select-sub.svg"
                width: (Constants.GridWidth / 2) - Constants.DefaultMargin;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
        }
        Label {
            id: fullSelectCommandsLabel;
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
                top: fullModeRow.bottom;
            }
            font.pixelSize: Constants.LargeFontSize;
            font.bold: true;
            height: Constants.GridHeight / 2;
            text: "Select:";
        }
        Item {
            id: fullSelectCommands;
            width: childrenRect.width;
            height: Constants.GridHeight;
            anchors {
                horizontalCenter: parent.horizontalCenter;
                top: fullSelectCommandsLabel.bottom;
            }
            Button {
                id: selectAllFull;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.margins: Constants.DefaultMargin;
                text: "All";
                textColor: "black";
                color: "#63ffffff";
                border.width: 1;
                border.color: "silver";
                radius: Constants.DefaultMargin;
                width: (Constants.GridWidth * 2 / 3) - Constants.DefaultMargin;
                height: textSize + Constants.DefaultMargin * 2
            }
            Button {
                id: selectInvertFull;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectAllFull.right;
                anchors.margins: Constants.DefaultMargin;
                text: "Invert";
                textColor: "black";
                color: "#63ffffff";
                border.width: 1;
                border.color: "silver";
                radius: Constants.DefaultMargin;
                width: (Constants.GridWidth * 2 / 3) - Constants.DefaultMargin;
                height: textSize + Constants.DefaultMargin * 2
            }
            Button {
                id: selectOpaqueFull;
                anchors.verticalCenter: parent.verticalCenter;
                anchors.left: selectInvertFull.right;
                anchors.margins: Constants.DefaultMargin;
                text: "Opaque";
                textColor: "black";
                color: "#63ffffff";
                border.width: 1;
                border.color: "silver";
                radius: Constants.DefaultMargin;
                width: (Constants.GridWidth * 2 / 3) - Constants.DefaultMargin;
                height: textSize + Constants.DefaultMargin * 2
            }
        }
        Label {
            id: fullEditingLabel;
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
                top: fullSelectCommands.bottom;
            }
            font.pixelSize: Constants.LargeFontSize;
            font.bold: true;
            height: Constants.GridHeight / 2;
            text: "Edit:";
        }
        Column {
            anchors {
                top: fullEditingLabel.bottom;
                left: parent.left;
                right: parent.right;
            }
            spacing: Constants.DefaultMargin;
            Item {
                width: fullItem.width;
                height: featherTxt.height;
                PanelTextField {
                    id: featherTxt;
                    anchors {
                        left: parent.left;
                        right: featherImg.left;
                    }
                    placeholder: "Feather";
                    text: "0.0"
                }
                Image {
                    id: featherImg;
                    anchors {
                        right: parent.right;
                        rightMargin: Constants.DefaultMargin;
                        verticalCenter: featherTxt.verticalCenter;
                    }
                    height: parent.height - (Constants.DefaultMargin * 2);
                    width: height;
                    source: "../images/svg/icon-select-apply.svg";
                    smooth: true;
                }
            }
            Item {
                width: fullItem.width;
                height: growTxt.height;
                PanelTextField {
                    id: growTxt;
                    anchors {
                        left: parent.left;
                        right: growImg.left;
                    }
                    placeholder: "Grow";
                    text: "0.0"
                }
                Image {
                    id: growImg;
                    anchors {
                        right: parent.right;
                        rightMargin: Constants.DefaultMargin;
                        verticalCenter: growTxt.verticalCenter;
                    }
                    height: parent.height - (Constants.DefaultMargin * 2);
                    width: height;
                    source: "../images/svg/icon-select-apply.svg";
                    smooth: true;
                }
            }
            Item {
                width: fullItem.width;
                height: borderTxt.height;
                PanelTextField {
                    id: borderTxt;
                    anchors {
                        left: parent.left;
                        right: borderImg.left;
                    }
                    placeholder: "Border";
                    text: "0.0"
                }
                Image {
                    id: borderImg;
                    anchors {
                        right: parent.right;
                        rightMargin: Constants.DefaultMargin;
                        verticalCenter: borderTxt.verticalCenter;
                    }
                    height: parent.height - (Constants.DefaultMargin * 2);
                    width: height;
                    source: "../images/svg/icon-select-apply.svg";
                    smooth: true;
                }
            }
            Item {
                width: fullItem.width;
                height: scaleTxt.height;
                PanelTextField {
                    id: scaleTxt;
                    anchors {
                        left: parent.left;
                        right: scaleImg.left;
                    }
                    placeholder: "Scale";
                    text: "0.0"
                }
                Image {
                    id: scaleImg;
                    anchors {
                        right: parent.right;
                        rightMargin: Constants.DefaultMargin;
                        verticalCenter: scaleTxt.verticalCenter;
                    }
                    height: parent.height - (Constants.DefaultMargin * 2);
                    width: height;
                    source: "../images/svg/icon-select-apply.svg";
                    smooth: true;
                }
            }
        }
    }

    dragDelegate: Component {
        Rectangle {
            color: Constants.Theme.QuaternaryColor;
            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
            radius: 8;

            Label {
                anchors.centerIn: parent;

                text: "Select";
                color: "white";

                font.pixelSize: Constants.DefaultFontSize;
            }
        }
    }
}
