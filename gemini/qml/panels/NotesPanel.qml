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
    id: base;
    property QtObject canvas: null;
    opacity: parent.checked ? 1 : 0;
    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
    anchors {
        top: parent.bottom;
        right: parent.right;
        topMargin: 4;
    }
    height: Constants.GridHeight * 5;
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
            text: "ADD NOTE";
            color: "#5b6573";
            font.pixelSize: Constants.SmallFontSize
            font.bold: true;
        }
    }
    Flickable {
        id: noteTemplatesFlickable;
        anchors {
            top: titleBar.bottom;
            left: parent.left;
            right: parent.right;
            bottom: customNote.top;
        }
        clip: true;
        contentHeight: noteTemplatesColumn.height;
        Column {
            id: noteTemplatesColumn;
            width: base.width;
            height: childrenRect.height;
            Item {
                width: parent.width;
                height: Constants.GridHeight;
                Flow {
                    anchors.fill: parent;
                    spacing: Constants.DefaultMargin;
                    Repeater {
                        model: ListModel {
                            ListElement { image: "Sticker-ThumbsUp.svg"; }
                            ListElement { image: "Sticker-Feather.svg"; }
                        }
                        Image {
                            width: Constants.GridHeight - Constants.DefaultMargin * 2;
                            height: width;
                            source: Settings.theme.image(model.image);
                            sourceSize.width: width > height ? height : width;
                            sourceSize.height: width > height ? height : width;
                            MouseArea {
                                anchors.fill: parent;
                                onClicked: base.canvas.addSticker(Settings.theme.image(model.image));
                            }
                        }
                    }
                }
            }
            Repeater {
                model: ListModel {
                    ListElement { text: "Check Spelling"; color: "red"; }
                    ListElement { text: "Needs more support"; color: "red"; }
                    ListElement { text: "Go deeper, perhaps"; color: "orange"; }
                    ListElement { text: "Great point!"; color: "green"; }
                    ListElement { text: "Good use of vocabulary!"; color: "green"; }
                    ListElement { text: "Nice!"; color: "green"; }
                    ListElement { text: "Well done!"; color: "green"; }
                    ListElement { text: "Splendid!"; color: "green"; }
                    ListElement { text: "Smashing!"; color: "green"; }
                }
                Item {
                    height: Constants.GridHeight / 2;
                    width: base.width;
                    Rectangle {
                        width: base.width;
                        height: 1;
                        color: "#e8e9ea";
                        opacity: 0.7;
                    }
                    Row {
                        height: Constants.GridHeight / 2;
                        width: base.width;
                        spacing: 3;
                        Item { height: parent.height; width: 4; }
                        Rectangle { width: 1; height: parent.height * 0.5; color: "#22282f"; opacity: 0.7; anchors.verticalCenter: parent.verticalCenter; }
                        Rectangle { width: 1; height: parent.height * 0.5; color: "#22282f"; opacity: 0.7; anchors.verticalCenter: parent.verticalCenter; }
                        Rectangle { width: 1; height: parent.height * 0.5; color: "#22282f"; opacity: 0.7; anchors.verticalCenter: parent.verticalCenter; }
                        Item { height: parent.height; width: 4; }
                        Label {
                            anchors.verticalCenter: parent.verticalCenter;
                            width: parent.width - parent.height * 1.5;
                            text: model.text;
                            color: "#5b6573";
                        }
                        Rectangle {
                            anchors.verticalCenter: parent.verticalCenter;
                            radius: parent.height / 8;
                            height: parent.height / 4;
                            width: height;
                            color: model.color;
                        }
                    }
                }
            }
        }
    }
    ScrollDecorator { flickableItem: noteTemplatesFlickable; anchors.fill: noteTemplatesFlickable; }
    Item {
        id: customNote;
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        height: Constants.GridHeight;
        Rectangle {
            anchors.top: parent.top;
            width: base.width;
            height: 1;
            color: "#e8e9ea";
            opacity: 0.7;
        }
        Button {
            anchors.centerIn: parent;
            height: textSize + Constants.DefaultMargin * 2;
            width: parent.width * 0.7;
            text: "Add a custom note";
            textColor: "#00adf5";
            image: Settings.theme.icon("add-black");
        }
    }
}
