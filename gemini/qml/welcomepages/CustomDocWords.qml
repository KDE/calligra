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

import QtQuick 2.0
import org.calligra 1.0
import "../components"

Page {
    id: base;
    property string pageName: "CustomDocWords";
    Item {
        anchors {
            top: parent.top;
            left: parent.horizontalCenter;
            right: parent.right;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
            bottomMargin: Constants.DefaultMargin * 2 + createDocButton.height;
        }
        Rectangle {
            id: singlePageVisualiser;
            opacity: facingCheck.checked ? 0 : 0.9;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            anchors.centerIn: parent;
            property double scale: widthInput.text / heightInput.text;
            property bool landscapeMode: landscapeCheck.checked ? widthInput.text < heightInput.text : heightInput.text < widthInput.text;
            height: landscapeMode ? (parent.height - Constants.DefaultMargin) * scale : (parent.height - Constants.DefaultMargin);
            width: landscapeMode ? (parent.width - Constants.DefaultMargin) : (parent.width - Constants.DefaultMargin) * scale;
            color: "white";
            border {
                color: "silver";
                width: 1;
            }
            Item {
                anchors {
                    fill: parent;
                    topMargin: marginTop.value;
                    leftMargin: marginLeft.value;
                    rightMargin: marginRight.value;
                    bottomMargin: marginBottom.value;
                }
                Row {
                    spacing: columnSpacing.value;
                    Repeater {
                        model: columnCount.value;
                        Rectangle {
                            height: singlePageVisualiser.height - marginTop.value - marginBottom.value;
                            width: ((singlePageVisualiser.width - marginLeft.value - marginRight.value) / columnCount.value) - (columnSpacing.value * (columnCount.value - 1) / columnCount.value);
                            color: "transparent";
                            opacity: 0.5;
                            border {
                                width: 1;
                                color: "silver";
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: leftPageVisualiser;
            opacity: facingCheck.checked ? 0.9 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            anchors {
                verticalCenter: parent.verticalCenter;
                right: parent.horizontalCenter;
            }
            property double scale: widthInput.text / heightInput.text;
            property bool landscapeMode: landscapeCheck.checked ? widthInput.text < heightInput.text : heightInput.text < widthInput.text;
            height: landscapeMode ? ((parent.height/2) - Constants.DefaultMargin) * scale : ((parent.height/2) - Constants.DefaultMargin);
            width: landscapeMode ? ((parent.width/2)- Constants.DefaultMargin) : ((parent.width/2) - Constants.DefaultMargin) * scale;
            color: "white";
            border {
                color: "silver";
                width: 1;
            }
            Item {
                anchors {
                    fill: parent;
                    topMargin: marginTop.value;
                    leftMargin: marginRight.value;
                    rightMargin: marginLeft.value;
                    bottomMargin: marginBottom.value;
                }
                Row {
                    spacing: columnSpacing.value;
                    Repeater {
                        model: columnCount.value;
                        Rectangle {
                            height: leftPageVisualiser.height - marginTop.value - marginBottom.value;
                            width: ((leftPageVisualiser.width - marginLeft.value - marginRight.value) / columnCount.value) - (columnSpacing.value * (columnCount.value - 1) / columnCount.value);
                            color: "transparent";
                            opacity: 0.5;
                            border {
                                width: 1;
                                color: "silver";
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: rightPageVisualiser;
            opacity: facingCheck.checked ? 0.9 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.horizontalCenter;
            }
            property double scale: widthInput.text / heightInput.text;
            property bool landscapeMode: landscapeCheck.checked ? widthInput.text < heightInput.text : heightInput.text < widthInput.text;
            height: landscapeMode ? ((parent.height/2) - Constants.DefaultMargin) * scale : ((parent.height/2) - Constants.DefaultMargin);
            width: landscapeMode ? ((parent.width/2)- Constants.DefaultMargin) : ((parent.width/2) - Constants.DefaultMargin) * scale;
            color: "white";
            border {
                color: "silver";
                width: 1;
            }
            Item {
                anchors {
                    fill: parent;
                    topMargin: marginTop.value;
                    leftMargin: marginLeft.value;
                    rightMargin: marginRight.value;
                    bottomMargin: marginBottom.value;
                }
                Row {
                    spacing: columnSpacing.value;
                    Repeater {
                        model: columnCount.value;
                        Rectangle {
                            height: rightPageVisualiser.height - marginTop.value - marginBottom.value;
                            width: ((rightPageVisualiser.width - marginLeft.value - marginRight.value) / columnCount.value) - (columnSpacing.value * (columnCount.value - 1) / columnCount.value);
                            color: "transparent";
                            opacity: 0.5;
                            border {
                                width: 1;
                                color: "silver";
                            }
                        }
                    }
                }
            }
        }
    }
    Item {
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.horizontalCenter;
            bottom: parent.bottom;
        }
        Column {
            width: parent.width;
            Label {
                width: parent.width;
                text: "Unit:"
            }
            ExpandingListView {
                id: unitList;
                width: parent.width;
                expandedHeight: Constants.GridHeight * 3;
                currentIndex: 6;
                model: ListModel {
                    id: unitModel;
                    ListElement { text: "Millimeters (mm)"; unit: "mm"; }
                    ListElement { text: "Centimeters (cm)"; unit: "cm"; }
                    ListElement { text: "Decimeters (dm)"; unit: "dm"; }
                    ListElement { text: "Inches (in)"; unit: "in"; }
                    ListElement { text: "Pica (pi)"; unit: "pi"; }
                    ListElement { text: "Cicero (cc)"; unit: "cc"; }
                    ListElement { text: "Points (pt)"; unit: "pt"; }
                }
            }
            Label {
                width: parent.width;
                text: "Size:"
            }
            ExpandingListView {
                id: paperSizeList;
                width: parent.width;
                expandedHeight: Constants.GridHeight * 6;
                currentIndex: 7;
                onCurrentIndexChanged: {
                    if(widthInput !== null) {
                        widthInput.text = paperSizeModel.get(currentIndex).width;
                        heightInput.text = paperSizeModel.get(currentIndex).height;
                    }
                }
                model: ListModel {
                    id: paperSizeModel;
                    ListElement { text: "Custom"; name: "Custom"; width: 210.0; height: 297.0; }
                    ListElement { text: "Screen"; name: "Screen"; width: 210.0; height: 297.0; }
                    ListElement { isCategory: true; text: "ISO formats"; }
                    ListElement { text: "ISO A0"; name: "A0"; width: 841.0; height: 1189.0; }
                    ListElement { text: "ISO A1"; name: "A1"; width: 594.0; height: 841.0; }
                    ListElement { text: "ISO A2"; name: "A2"; width: 420.0; height: 594.0; }
                    ListElement { text: "ISO A3"; name: "A3"; width: 297.0; height: 420.0; }
                    ListElement { text: "ISO A4"; name: "A4"; width: 210.0; height: 297.0; }
                    ListElement { text: "ISO A5"; name: "A5"; width: 148.0; height: 210.0; }
                    ListElement { text: "ISO A6"; name: "A6"; width: 105.0; height: 148.0; }
                    ListElement { text: "ISO A7"; name: "A7"; width: 74.0; height: 105.0; }
                    ListElement { text: "ISO A8"; name: "A8"; width: 52.0; height: 74.0; }
                    ListElement { text: "ISO A9"; name: "A9"; width: 37.0; height: 52.0; }
                    ListElement { text: "ISO B0"; name: "B0"; width: 1030.0; height: 1456.0; }
                    ListElement { text: "ISO B1"; name: "B1"; width: 728.0; height: 1030.0; }
                    ListElement { text: "ISO B2"; name: "B2"; width: 515.0; height: 728.0; }
                    ListElement { text: "ISO B3"; name: "B3"; width: 364.0; height: 515.0; }
                    ListElement { text: "ISO B4"; name: "B4"; width: 257.0; height: 364.0; }
                    ListElement { text: "ISO B5"; name: "B5"; width: 182.0; height: 257.0; }
                    ListElement { text: "ISO B6"; name: "B6"; width: 128.0; height: 182.0; }
                    ListElement { text: "ISO B10"; name: "B10"; width: 32.0; height: 45.0; }
                    ListElement { text: "ISO C5"; name: "C5"; width: 163.0; height: 229.0; }
                    ListElement { text: "ISO DL"; name: "DL"; width: 110.0; height: 220.0; }
                    ListElement { isCategory: true; text: "US formats"; }
                    ListElement { text: "US Letter"; name: "Letter"; width: 215.9; height: 279.4; }
                    ListElement { text: "US Legal"; name: "Legal"; width: 215.9; height: 355.6; }
                    ListElement { text: "US Executive"; name: "Executive"; width: 191.0; height: 254.0; }
                    ListElement { text: "US Folio"; name: "Folio"; width: 210.0; height: 330.0; }
                    ListElement { text: "US Ledger"; name: "Ledger"; width: 432.0; height: 279.0; }
                    ListElement { text: "US Tabloid"; name: "Tabloid"; width: 279.0; height: 432.0; }
                    ListElement { test: "US Common 10"; name: "Comm10"; width: 105.0; height: 241.0; }
                }
            }
            Item {
                width: parent.width;
                height: opacity > 0 ? childrenRect.height : 0;
                Behavior on height { PropertyAnimation { duration: Constants.AnimationDuration; } }
                opacity: paperSizeList.currentIndex === 0 ? 1 : 0;
                Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                clip: true;
                PanelTextField {
                    id: widthInput;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.horizontalCenter;
                    }
                    width: parent.width;
                    placeholder: landscapeCheck.checked ? "Height" : "Width";
                    validator: DoubleValidator{ bottom: 1; top: 999999; decimals: 2; }
                    numeric: true;
                }
                PanelTextField {
                    id: heightInput;
                    anchors {
                        top: parent.top;
                        left: parent.horizontalCenter;
                        right: parent.right;
                    }
                    width: parent.width;
                    placeholder: landscapeCheck.checked ? "Width" : "Height";
                    validator: DoubleValidator{ bottom: 1; top: 999999; decimals: 2; }
                    numeric: true;
                }
            }
            Label {
                width: parent.width;
                text: "Orientation:";
            }
            Item {
                height: Constants.GridHeight / 2;
                width: parent.width;
                CohereButton {
                    id: portraitCheck;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.horizontalCenter;
                        bottom: parent.bottom;
                    }
                    text: "Portrait";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    checkedColor: "#D2D4D5";
                    onClicked: portraitCheck.checked = true;
                    checked: true;
                }
                CohereButton {
                    id: landscapeCheck;
                    anchors {
                        top: parent.top;
                        left: parent.horizontalCenter;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    text: "Landscape";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    checkedColor: "#D2D4D5";
                    onClicked: portraitCheck.checked = false;
                    checked: !portraitCheck.checked;
                }
            }
            Label {
                width: parent.width;
                text: "Facing pages:";
            }
            Item {
                height: Constants.GridHeight / 2;
                width: parent.width;
                CohereButton {
                    id: nonFacingCheck;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.horizontalCenter;
                        bottom: parent.bottom;
                    }
                    text: "Single sided";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    checkedColor: "#D2D4D5";
                    onClicked: nonFacingCheck.checked = true;
                    checked: true;
                }
                CohereButton {
                    id: facingCheck;
                    anchors {
                        top: parent.top;
                        left: parent.horizontalCenter;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    text: "Facing";
                    textColor: "#5b6573";
                    textSize: Settings.theme.adjustedPixel(18);
                    checkedColor: "#D2D4D5";
                    onClicked: nonFacingCheck.checked = false;
                    checked: !nonFacingCheck.checked;
                }
            }
            Label {
                width: parent.width;
                text: "Columns:";
            }
            Row {
                width: parent.width;
                RangeInput {
                    id: columnCount;
                    width: parent.width / 2;
                    placeholder: "Amount";
                    min: 1; max: 6; decimals: 0;
                    value: 1;
                }
                RangeInput {
                    id: columnSpacing;
                    width: parent.width / 2;
                    placeholder: "Spacing";
                    useExponentialValue: true;
                    min: 0;
                    max: (heightInput.text > widthInput.text ? heightInput.text : widthInput.text) / columnCount.value;
                    decimals: 2;
                    // This looks odd, perhaps. It does, however, ensure that when we start, we are not
                    // setting the value to zero because of an order-of-initialisation issue meaning the
                    // column count is zero, which makes the interpreter simply set max to 0, so we end
                    // up with value reset to 0 as well. So, doing it like this fixes that silly little
                    // issue
                    Component.onCompleted: value = 20;
                }
            }
            Row {
                width: parent.width;
                height: childrenRect.height;
                Item {
                    height: 10;
                    width: parent.width / 3;
                }
                RangeInput {
                    id: marginTop;
                    width: parent.width / 3;
                    placeholder: "Top";
                    useExponentialValue: true;
                    min: 1; max: 999; decimals: 2;
                    value: 20;
                }
            }
            Row {
                width: parent.width;
                height: childrenRect.height;
                RangeInput {
                    id: marginLeft;
                    width: parent.width / 3;
                    placeholder: facingCheck.checked ? "Binding" : "Left";
                    useExponentialValue: true;
                    min: 1; max: 999; decimals: 2;
                    value: 20;
                }
                Label {
                    height: marginLeft.height;
                    width: parent.width / 3;
                    text: "Margins";
                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: Text.AlignHCenter;
                }
                RangeInput {
                    id: marginRight;
                    width: parent.width / 3;
                    placeholder: facingCheck.checked ? "Outside" : "Right";
                    useExponentialValue: true;
                    min: 1; max: 999; decimals: 2;
                    value: 20;
                }
            }
            Row {
                width: parent.width;
                height: childrenRect.height;
                Item {
                    height: 10;
                    width: parent.width / 3;
                }
                RangeInput {
                    id: marginBottom;
                    width: parent.width / 3;
                    placeholder: "Bottom";
                    useExponentialValue: true;
                    min: 1; max: 999; decimals: 2;
                    value: 20;
                }
            }
        }
    }
    CohereButton {
        id: createDocButton;
        anchors {
            right: parent.right;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
        }
        text: "Create Document";
        textColor: "white";
        textSize: Settings.theme.adjustedPixel(18);
        color: "#4e5359";
        onClicked: {
            var queryString = "newfile:///";
            queryString += "?mimetype=" + WORDS_MIME_TYPE;
            queryString += "&columncount=" + columnCount.value;
            queryString += "&columngap=" + columnSpacing.value;
            queryString += "&unit=" + unitModel.get(unitList.currentIndex).unit;
            queryString += "&pageformat=" + paperSizeModel.get(paperSizeList.currentIndex).name;
            queryString += "&pageorientation=" + (portraitCheck.checked ? 0 : 1);
            queryString += "&height=" + heightInput.text;
            queryString += "&width=" + widthInput.text;
            queryString += "&topmargin=" + marginTop.value;
            queryString += "&leftmargin=" + marginLeft.value;
            queryString += "&rightmargin=" + marginRight.value;
            queryString += "&bottommargin=" + marginBottom.value;
            queryString += "&facingpages=" + (facingCheck.checked ? 1 : 0);
            baseLoadingDialog.visible = true;
            openFile(queryString);
        }
    }
}
