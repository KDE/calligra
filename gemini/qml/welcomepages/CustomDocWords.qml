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
import org.calligra 1.0
import "../components"

Page {
    id: base;
    property string pageName: "CustomDocWords";
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
                width: parent.width;
                expandedHeight: Constants.GridHeight * 3;
                currentIndex: 6;
                model: ListModel {
                    ListElement { text: "Millimeters (mm)"; }
                    ListElement { text: "Centimeters (cm)"; }
                    ListElement { text: "Decimeters (dm)"; }
                    ListElement { text: "Inches (in)"; }
                    ListElement { text: "Pica (pi)"; }
                    ListElement { text: "Cicero (cc)"; }
                    ListElement { text: "Points (pt)"; }
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
                model: ListModel {
                    ListElement { text: "Custom"; }
                    ListElement { text: "Screen"; }
                    ListElement { isCategory: true; text: "ISO formats"; }
                    ListElement { text: "ISO A0"; }
                    ListElement { text: "ISO A1"; }
                    ListElement { text: "ISO A2"; }
                    ListElement { text: "ISO A3"; }
                    ListElement { text: "ISO A4"; }
                    ListElement { text: "ISO A5"; }
                    ListElement { text: "ISO A6"; }
                    ListElement { text: "ISO A7"; }
                    ListElement { text: "ISO A8"; }
                    ListElement { text: "ISO A9"; }
                    ListElement { text: "ISO B0"; }
                    ListElement { text: "ISO B1"; }
                    ListElement { text: "ISO B2"; }
                    ListElement { text: "ISO B3"; }
                    ListElement { text: "ISO B4"; }
                    ListElement { text: "ISO B5"; }
                    ListElement { text: "ISO B6"; }
                    ListElement { text: "ISO B7"; }
                    ListElement { text: "ISO B8"; }
                    ListElement { text: "ISO B9"; }
                    ListElement { text: "ISO B10"; }
                    ListElement { text: "ISO C5"; }
                    ListElement { text: "ISO DL"; }
                    ListElement { isCategory: true; text: "US formats"; }
                    ListElement { text: "US Letter"; }
                    ListElement { text: "US Legal"; }
                    ListElement { text: "US Executive"; }
                    ListElement { text: "US Folio"; }
                    ListElement { text: "US Ledger"; }
                    ListElement { text: "US Tabloid"; }
                }
            }
            Item {
                width: parent.width;
                height: opacity > 0 ? childrenRect.height : 0;
                Behavior on height { PropertyAnimation { duration: Constants.AnimationDuration; } }
                opacity: paperSizeList.currentIndex === 0 ? 1 : 0;
                Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                clip: true;
                Column {
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.horizontalCenter;
                    }
                    height: childrenRect.height;
                    Label {
                        width: parent.width;
                        text: "Width:";
                    }
                    PanelTextField {
                        width: parent.width;
                        text: "1234";
                    }
                }
                Column {
                    anchors {
                        top: parent.top;
                        left: parent.horizontalCenter;
                        right: parent.right;
                    }
                    height: childrenRect.height;
                    Label {
                        width: parent.width;
                        text: "Height:";
                    }
                    PanelTextField {
                        width: parent.width;
                        text: "1234";
                    }
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
            RangeInput {
                id: columnCount;
                width: parent.width;
                placeholder: "Amount";
                min: 1; max: 99; decimals: 0;
                value: 1;
            }
            RangeInput {
                id: columnSpacing;
                width: parent.width;
                placeholder: "Spacing";
                useExponentialValue: true;
                min: 0; max: 999; decimals: 2;
                value: 20;
            }
        }
        CohereButton {
            anchors {
                horizontalCenter: parent.horizontalCenter;
                bottom: parent.bottom;
                margins: Constants.DefaultMargin;
            }
            text: "Create Document";
            textColor: "white";
            textSize: Settings.theme.adjustedPixel(18);
            color: "#4e5359";
        }
    }
}
