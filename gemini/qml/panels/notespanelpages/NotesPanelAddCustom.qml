/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import "../../components"
import org.calligra 1.0

Item {
    id: base;
    property QtObject canvas: null;
    property string titleText;
    Rectangle {
        id: titleBar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight;
        color: "#e8e9ea";
        Rectangle {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            height: 1;
            color: "black";
            opacity: 0.5;
        }
        Button {
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }
            height: parent.height - Constants.DefaultMargin * 2;
            width: height;
            image: Settings.theme.icon("SVG-Icon-MenuBack-1");
            onClicked: notesPageStack.pop();
        }
        Button {
            anchors {
                right: parent.right;
                rightMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }
            height: parent.height - Constants.DefaultMargin * 2;
            width: height;
            image: Settings.theme.icon("SVG-Icon-OK-1");
            enabled: noteText.text !== "";
            onClicked: {
                base.canvas.addNote(noteText.text, colorModel.actualSelectedColor, Settings.theme.image("intel-Words-Note-Circle-%1.svg".arg(colorModel.selectedColor)));
                toolManager.requestToolChange("InteractionTool");
                viewLoader.item.navigateMode = false;
                notesPageStack.pop();
            }
        }
        Label {
            anchors {
                left: parent.left;
                right: parent.right;
                verticalCenter: parent.verticalCenter;
            }
            text: base.titleText;
            color: "#5b6573";
            font.pixelSize: Constants.SmallFontSize
            font.bold: true;
            horizontalAlignment: Text.AlignHCenter;
        }
    }
    Item {
        id: labelContainer;
        anchors {
            top: titleBar.bottom;
            left: parent.left;
            right: parent.right;
        }
        height: titleText === "ADD CUSTOM NOTE" ? Constants.GridHeight : 0;
        clip: true;
        Label {
            anchors {
                verticalCenter: parent.verticalCenter;
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
            }
            text: "Label:";
            color: "#5b6573";
        }
        Row {
            anchors {
                right: parent.right;
                rightMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }
            spacing: Constants.DefaultMargin;
            ListModel {
                id: colorModel;
                property string selectedColor: "Green";
                property string actualSelectedColor: "#29b618";
                ListElement { color: "Red"; actualColor: "#fd5134"; selected: false; }
                ListElement { color: "Yellow"; actualColor: "#ffb20c"; selected: true; }
                ListElement { color: "Green"; actualColor: "#29b618"; selected: false; }
            }
            Repeater {
                model: colorModel;
                Image {
                    height: Constants.GridHeight / 2;
                    width: height;
                    source: Settings.theme.icon("SVG-Label-%1-1".arg(model.color));
                    sourceSize.width: width > height ? height : width;
                    sourceSize.height: width > height ? height : width;
                    Image {
                        anchors.fill: parent;
                        opacity: model.selected ? 1 : 0;
                        Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                        source: Settings.theme.icon("SVG-Label-SelectedRing-1");
                        sourceSize.width: width > height ? height : width;
                        sourceSize.height: width > height ? height : width;
                    }
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            colorModel.setProperty(0, "selected", index === 0);
                            colorModel.setProperty(1, "selected", index === 1);
                            colorModel.setProperty(2, "selected", index === 2);
                            colorModel.selectedColor = model.color;
                            colorModel.actualSelectedColor = model.actualColor;
                        }
                    }
                }
            }
        }
    }
    Item {
        anchors {
            top: labelContainer.bottom;
            left: parent.left;
            right: parent.right;
            bottom: keyboard.top;
        }
        Label {
            anchors {
                top: noteText.top;
                left: noteText.left;
            }
            opacity: noteText.text === "" ? 0.6 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            text: "Tap to enter your note";
            font: noteText.font;
            color: "#5b6573";
        }
        TextEdit {
            id: noteText;
            anchors {
                fill: parent;
                margins: Constants.DefaultMargin;
            }
            font: Settings.theme.font("application");
            color: "#5b6573";
        }
    }
    Item {
        id: keyboard;
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        height: Constants.GridHeight * 4;
        VirtualKeyboard {
            anchors.fill: parent;
            state: "visible";
        }
    }
}
