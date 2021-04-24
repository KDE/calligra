/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import org.kde.kirigami 2.14 as Kirigami
import QtQuick.Controls 2.15 as QQC2
import org.calligra 1.0
import "../components"

Kirigami.Page {
    id: base;
    property string pageName: "CustomDocWords";
    title: i18n("Create Custom Document")
    Item {
        anchors {
            top: parent.top;
            left: parent.horizontalCenter;
            right: parent.right;
            bottom: parent.bottom;
            margins: Kirigami.Units.largeSpacing
            bottomMargin: Kirigami.Units.largeSpacing + createDocButton.height;
        }
        Rectangle {
            id: singlePageVisualiser;
            opacity: facingCheck.checked ? 0 : 0.9;
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
            anchors.centerIn: parent;
            property double scale: widthInput.text / heightInput.text;
            property bool landscapeMode: landscapeCheck.checked ? widthInput.text < heightInput.text : heightInput.text < widthInput.text;
            height: landscapeMode ? (parent.height - Constants.DefaultMargin) * scale : (parent.height - Constants.DefaultMargin);
            width: landscapeMode ? (parent.width - Constants.DefaultMargin) : (parent.width - Constants.DefaultMargin) * scale;
            color: Kirigami.Theme.backgroundColor;
            Kirigami.Theme.colorSet: Kirigami.Theme.View
            Kirigami.Theme.inherit: false
            border {
                color: Kirigami.Theme.alternateBackgroundColor;
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
                                color: Kirigami.Theme.textColor;
                            }
                        }
                    }
                }
            }
        }
        Rectangle {
            id: leftPageVisualiser;
            opacity: facingCheck.checked ? 0.9 : 0;
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
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
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
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
            QQC2.Label {
                width: parent.width;
                text: i18n("Unit:")
            }
            QQC2.ComboBox {
                id: unitList;
                width: parent.width;
                model: ListModel {
                    id: unitModel;
                }
                textRole: "text"
                valueRole: "unit"
                Component.onCompleted: {
                    unitModel.append({ "text": i18n("Millimeters (mm)"), "unit": "mm" });
                    unitModel.append({ "text": i18n("Centimeters (cm)"), "unit": "cm" });
                    unitModel.append({ "text": i18n("Decimeters (dm)"), "unit": "dm" });
                    unitModel.append({ "text": i18n("Inches (in)"), "unit": "in" });
                    unitModel.append({ "text": i18n("Pica (pi)"), "unit": "pi" });
                    unitModel.append({ "text": i18n("Cicero (cc)"), "unit": "cc" });
                    unitModel.append({ "text": i18n("Points (pt)"), "unit": "pt" });
                    currentIndex = 6;
                }
            }
            QQC2.Label {
                width: parent.width;
                text: i18n("Size:")
            }
            QQC2.ComboBox {
                id: paperSizeList;
                width: parent.width;
                textRole: "text"
                onCurrentIndexChanged: {
                    if(widthInput !== null) {
                        widthInput.text = paperSizeModel.get(currentIndex).width;
                        heightInput.text = paperSizeModel.get(currentIndex).height;
                    }
                }
                model: ListModel {
                    id: paperSizeModel;
                }
                Component.onCompleted: {
                    paperSizeModel.append({ "text": i18n("Custom"), name: "Custom", width: 210.0, height: 297.0 });
                    paperSizeModel.append({ "text": i18n("Screen"), name: "Screen", width: 210.0, height: 297.0 });
                    //paperSizeModel.append({ isCategory: true, text: "ISO formats" });
                    paperSizeModel.append({ "text": i18n("ISO A0"), name: "A0", width: 841.0, height: 1189.0 });
                    paperSizeModel.append({ "text": i18n("ISO A1"), name: "A1", width: 594.0, height: 841.0 });
                    paperSizeModel.append({ "text": i18n("ISO A2"), name: "A2", width: 420.0, height: 594.0 });
                    paperSizeModel.append({ "text": i18n("ISO A3"), name: "A3", width: 297.0, height: 420.0 });
                    paperSizeModel.append({ "text": i18n("ISO A4"), name: "A4", width: 210.0, height: 297.0 });
                    paperSizeModel.append({ "text": i18n("ISO A5"), name: "A5", width: 148.0, height: 210.0 });
                    paperSizeModel.append({ "text": i18n("ISO A6"), name: "A6", width: 105.0, height: 148.0 });
                    paperSizeModel.append({ "text": i18n("ISO A7"), name: "A7", width: 74.0, height: 105.0 });
                    paperSizeModel.append({ "text": i18n("ISO A8"), name: "A8", width: 52.0, height: 74.0 });
                    paperSizeModel.append({ "text": i18n("ISO A9"), name: "A9", width: 37.0, height: 52.0 });
                    paperSizeModel.append({ "text": i18n("ISO B0"), name: "B0", width: 1030.0, height: 1456.0 });
                    paperSizeModel.append({ "text": i18n("ISO B1"), name: "B1", width: 728.0, height: 1030.0 });
                    paperSizeModel.append({ "text": i18n("ISO B2"), name: "B2", width: 515.0, height: 728.0 });
                    paperSizeModel.append({ "text": i18n("ISO B3"), name: "B3", width: 364.0, height: 515.0 });
                    paperSizeModel.append({ "text": i18n("ISO B4"), name: "B4", width: 257.0, height: 364.0 });
                    paperSizeModel.append({ "text": i18n("ISO B5"), name: "B5", width: 182.0, height: 257.0 });
                    paperSizeModel.append({ "text": i18n("ISO B6"), name: "B6", width: 128.0, height: 182.0 });
                    paperSizeModel.append({ "text": i18n("ISO B10"), name: "B10", width: 32.0, height: 45.0 });
                    paperSizeModel.append({ "text": i18n("ISO C5"), name: "C5", width: 163.0, height: 229.0 });
                    paperSizeModel.append({ "text": i18n("ISO DL"), name: "DL", width: 110.0, height: 220.0 });
                    //paperSizeModel.append({ isCategory: true, text: "US formats" });
                    paperSizeModel.append({ "text": i18n("US Letter"), name: "Letter", width: 215.9, height: 279.4 });
                    paperSizeModel.append({ "text": i18n("US Legal"), name: "Legal", width: 215.9, height: 355.6 });
                    paperSizeModel.append({ "text": i18n("US Executive"), name: "Executive", width: 191.0, height: 254.0 });
                    paperSizeModel.append({ "text": i18n("US Folio"), name: "Folio", width: 210.0, height: 330.0 });
                    paperSizeModel.append({ "text": i18n("US Ledger"), name: "Ledger", width: 432.0, height: 279.0 });
                    paperSizeModel.append({ "text": i18n("US Tabloid"), name: "Tabloid", width: 279.0, height: 432.0 });
                    paperSizeModel.append({ "text": i18n("US Common 10"), name: "Comm10", width: 105.0, height: 241.0 });
                    currentIndex = 7;
                }
            }
            Item {
                width: parent.width;
                height: opacity > 0 ? childrenRect.height : 0;
                Behavior on height { NumberAnimation { duration: Kirigami.Units.shortDuration; easing.type: Easing.InOutQuad; } }
                opacity: paperSizeList.currentIndex === 0 ? 1 : 0;
                Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                clip: true;
                QQC2.TextField {
                    id: widthInput;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.horizontalCenter;
                    }
                    width: parent.width;
                    placeholderText: landscapeCheck.checked ? i18n("Height") : i18n("Width");
                    validator: DoubleValidator{ bottom: 1; top: 999999; decimals: 2; }
                    inputMethodHints: Qt.ImhFormattedNumbersOnly;
                }
                QQC2.TextField {
                    id: heightInput;
                    anchors {
                        top: parent.top;
                        left: parent.horizontalCenter;
                        right: parent.right;
                    }
                    width: parent.width;
                    placeholderText: landscapeCheck.checked ? i18n("Width") : i18n("Height");
                    validator: DoubleValidator{ bottom: 1; top: 999999; decimals: 2; }
                    inputMethodHints: Qt.ImhFormattedNumbersOnly;
                }
            }
            QQC2.Label {
                width: parent.width;
                text: i18n("Orientation:")
            }
            Item {
                height: Constants.GridHeight / 2;
                width: parent.width;
                QQC2.Button {
                    id: portraitCheck;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.horizontalCenter;
                        bottom: parent.bottom;
                    }
                    text: i18n("Portrait");
                    onClicked: portraitCheck.checked = true;
                    checked: true;
                }
                QQC2.Button {
                    id: landscapeCheck;
                    anchors {
                        top: parent.top;
                        left: parent.horizontalCenter;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    text: i18n("Landscape");
                    onClicked: portraitCheck.checked = false;
                    checked: !portraitCheck.checked;
                }
            }
            QQC2.Label {
                width: parent.width;
                text: i18n("Facing pages:");
            }
            Item {
                height: Constants.GridHeight / 2;
                width: parent.width;
                QQC2.Button {
                    id: nonFacingCheck;
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        right: parent.horizontalCenter;
                        bottom: parent.bottom;
                    }
                    text: i18n("Single sided");
                    onClicked: nonFacingCheck.checked = true;
                    checked: true;
                }
                QQC2.Button {
                    id: facingCheck;
                    anchors {
                        top: parent.top;
                        left: parent.horizontalCenter;
                        right: parent.right;
                        bottom: parent.bottom;
                    }
                    text: i18n("Facing");
                    onClicked: nonFacingCheck.checked = false;
                    checked: !nonFacingCheck.checked;
                }
            }
            QQC2.Label {
                width: parent.width;
                text: i18n("Columns:");
            }
            Row {
                width: parent.width;
                RangeInput {
                    id: columnCount;
                    width: parent.width / 2;
                    placeholder: i18n("Amount");
                    min: 1; max: 6; decimals: 0;
                    value: 1;
                }
                RangeInput {
                    id: columnSpacing;
                    width: parent.width / 2;
                    placeholder: i18n("Spacing");
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
                    placeholder: i18n("Top");
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
                    placeholder: facingCheck.checked ? i18n("Binding") : i18n("Left");
                    min: 1; max: 999; decimals: 2;
                    value: 20;
                }
                Label {
                    height: marginLeft.height;
                    width: parent.width / 3;
                    text: i18n("Margins");
                    verticalAlignment: Text.AlignVCenter;
                    horizontalAlignment: Text.AlignHCenter;
                }
                RangeInput {
                    id: marginRight;
                    width: parent.width / 3;
                    placeholder: facingCheck.checked ? i18n("Outside") : i18n("Right");
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
                    placeholder: i18n("Bottom");
                    min: 1; max: 999; decimals: 2;
                    value: 20;
                }
            }
        }
    }
    QQC2.Button {
        id: createDocButton;
        anchors {
            right: parent.right;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
        }
        text: i18n("Create Document");
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
            openFile(queryString);
        }
    }
}
