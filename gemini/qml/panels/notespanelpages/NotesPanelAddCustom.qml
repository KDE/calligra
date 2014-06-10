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
import "../../components"
import org.calligra 1.0

Item {
    id: base;
    property QtObject canvas: null;
    Rectangle {
        id: titleBar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight;
        color: "#e8e9ea";
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
                base.canvas.addNote(noteText.text, colorModel.selectedColor);
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
            text: "ADD CUSTOM NOTE";
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
        height: Constants.GridHeight;
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
                verticalCenter: parent.verticalCenter;
            }
            spacing: Constants.DefaultMargin;
            ListModel {
                id: colorModel;
                property string selectedColor: "Yellow";
                ListElement { color: "Red"; selected: false; }
                ListElement { color: "Yellow"; selected: true; }
                ListElement { color: "Green"; selected: false; }
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
//         VirtualKeyboard {
//             anchors.fill: parent;
//             state: "visible";
//         }
    }
}
