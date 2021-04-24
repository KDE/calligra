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
        Label {
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }
            text: "ADD NOTES";
            color: "#5b6573";
            font.pixelSize: Constants.SmallFontSize
            font.bold: true;
        }
        Row {
            anchors {
                right: parent.right;
                rightMargin: Constants.DefaultMargin;
                verticalCenter: parent.verticalCenter;
            }
            height: parent.height - Constants.DefaultMargin * 2;
            Button {
                height: parent.height;
                width: height;
                image: Settings.theme.icon("SVG-Icon-AddVoiceComment-1");
            }
            Button {
                height: parent.height;
                width: height;
                image: Settings.theme.icon("SVG-Icon-AddPhotoComment-1");
            }
            Button {
                height: parent.height;
                width: height;
                image: Settings.theme.icon("SVG-Icon-NotesSummary-1");
                onClicked: notesPageStack.push(summaryView);
                Image {
                    anchors.fill: parent;
                    source: Settings.theme.icon("SVG-NotesSummary-Counter-1");
                    sourceSize.width: width > height ? height : width;
                    sourceSize.height: width > height ? height : width;
                    Text {
                        anchors {
                            right: parent.right;
                            top: parent.top;
                        }
                        height: parent.height * 0.57291667;
                        width: parent.width * 0.45833333;
                        color: "white";
                        horizontalAlignment: Text.AlignHCenter;
                        verticalAlignment: Text.AlignVCenter;
                        font: Settings.theme.font("small");
                        text: base.canvas ? base.canvas.notes.count : "";
                    }
                }
            }
        }
    }
    Item {
        id: stickerTemplates;
        anchors {
            top: titleBar.bottom;
            left: parent.left;
            right: parent.right;
        }
        height: Constants.GridHeight * 2.4;
        GridView {
            id: stickerView;
            anchors.fill: parent;
            clip: true;
            cellWidth: width / 3;
            cellHeight: (height / 2) - Constants.DefaultMargin;
            snapMode: GridView.SnapToRow;
            flow: GridView.TopToBottom;
            model: ListModel {
                ListElement { image: "Sticker-ThumbsUp.svg"; }
                ListElement { image: "Sticker-Feather.svg"; }
                ListElement { image: "Sticker-Apple.svg"; }
                ListElement { image: "Sticker-ArrowTarget.svg"; }
                ListElement { image: "Sticker-Lightbulb.svg"; }
                ListElement { image: "Sticker-Ribbon.svg"; }
                ListElement { image: "Sticker-OK.svg"; }
                ListElement { image: "Sticker-A.svg"; }
                ListElement { image: "Sticker-B.svg"; }
                ListElement { image: "Sticker-C.svg"; }
                ListElement { image: "Sticker-D.svg"; }
                ListElement { image: "Sticker-F.svg"; }
            }
            delegate: Item {
                width: stickerView.cellWidth;
                height: stickerView.cellHeight;
                Image {
                    anchors.centerIn: parent;
                    height: parent.height - Constants.DefaultMargin;
                    width: height;
                    source: Settings.theme.image(model.image);
                    sourceSize.width: width > height ? height : width;
                    sourceSize.height: width > height ? height : width;
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            base.canvas.addSticker(Settings.theme.image(model.image));
                            toolManager.requestToolChange("InteractionTool");
                            viewLoader.item.navigateMode = false;
                        }
                    }
                }
            }
            ScrollDecorator { flickableItem: stickerView; }
        }
    }
    Flickable {
        id: noteTemplatesFlickable;
        anchors {
            top: stickerTemplates.bottom;
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
            Repeater {
                model: ListModel {
                    ListElement { text: "Check Spelling"; color: "#fd5134"; circleColor: "Red"; image: "intel-Words-Note-Circle-Red.svg"; }
                    ListElement { text: "Needs more support"; color: "#fd5134"; circleColor: "Red"; image: "intel-Words-Note-Circle-Red.svg"; }
                    ListElement { text: "Go deeper, perhaps"; color: "#ffb20c"; circleColor: "Yellow"; image: "intel-Words-Note-Circle-Yellow.svg"; }
                    ListElement { text: "Great point!"; color: "#29b618"; circleColor: "Green"; image: "intel-Words-Note-Circle-Green.svg"; }
                    ListElement { text: "Good use of vocabulary!"; color: "#29b618"; circleColor: "Green"; image: "intel-Words-Note-Circle-Green.svg"; }
                    ListElement { text: "Nice!"; color: "#29b618"; circleColor: "Green"; image: "intel-Words-Note-Circle-Green.svg"; }
                    ListElement { text: "Well done!"; color: "#29b618"; circleColor: "Green"; image: "intel-Words-Note-Circle-Green.svg"; }
                    ListElement { text: "Splendid!"; color: "#29b618"; circleColor: "Green"; image: "intel-Words-Note-Circle-Green.svg"; }
                    ListElement { text: "Smashing!"; color: "#29b618"; circleColor: "Green"; image: "intel-Words-Note-Circle-Green.svg"; }
                }
                Item {
                    height: Constants.GridHeight;
                    width: base.width;
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            base.canvas.addNote(model.text, model.color, Settings.theme.image(model.image));
                            toolManager.requestToolChange("InteractionTool");
                            viewLoader.item.navigateMode = false;
                        }
                    }
                    Rectangle {
                        width: base.width;
                        height: 1;
                        color: "#e8e9ea";
                        opacity: 0.7;
                    }
                    Row {
                        anchors.verticalCenter: parent.verticalCenter;
                        height: Constants.GridHeight / 2;
                        width: base.width;
                        Item { height: parent.height; width: Constants.DefaultMargin; }
                        Image {
                            height: parent.height;
                            width: height * 0.26470588;
                            source: Settings.theme.icon("SVG-Comment-DragHandle-1");
                            sourceSize.width: width > height ? height : width;
                            sourceSize.height: width > height ? height : width;
                        }
                        Item { height: parent.height; width: Constants.DefaultMargin * 2; }
                        Label {
                            anchors.verticalCenter: parent.verticalCenter;
                            width: parent.width - parent.height * 1.3 - Constants.DefaultMargin * 3;
                            text: model.text;
                            color: "#5b6573";
                        }
                        Image {
                            height: parent.height;
                            width: height;
                            source: Settings.theme.icon("SVG-Label-%1-1".arg(model.circleColor));
                            sourceSize.width: width > height ? height : width;
                            sourceSize.height: width > height ? height : width;
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
            anchors.fill: parent;
            color: "#e8e9ea";
            opacity: 0.7;
        }
        Rectangle {
            anchors {
                fill: parent;
                margins: Constants.DefaultMargin;
            }
            color: "white";
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    notesPageStack.customNoteTitleText = "ADD CUSTOM NOTE";
                    notesPageStack.push(customNoteView);
                }
            }
            Row {
                anchors.centerIn: parent;
                height: parent.height / 3;
                spacing: Constants.DefaultMargin;
                Image {
                    height: parent.height;
                    width: height;
                    source: Settings.theme.icon("SVG-AddComment-Inline-1");
                    sourceSize.width: width > height ? height : width;
                    sourceSize.height: width > height ? height : width;
                }
                Text {
                    anchors.verticalCenter: parent.verticalCenter;
                    width: paintedWidth;
                    font.pixelSize: parent.height;
                    color: "#00adf5";
                    text: "Add Custom Note";
                    opacity: 0.6;
                }
            }
        }
    }
}
