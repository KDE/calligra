/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.0
import "components"
import "panels"
import org.calligra 1.0
import org.kde.calligra 1.0 as Calligra
import org.kde.kirigami 2.7 as Kirigami

Kirigami.Page {
    id: base;
    property string pageName: "MainPage";
    // Not very Kirigami-ish, but we're just going to leave the base interaction for now
    topPadding: 0;
    leftPadding: 0;
    rightPadding: 0;
    bottomPadding: 0;
    function openFileReal(fileName) {
        if(Settings.currentFileClass === WORDS_MIME_TYPE) {
            viewLoader.sourceComponent = wordsView;
        } else if(Settings.currentFileClass === STAGE_MIME_TYPE) {
            viewLoader.sourceComponent = stageView;
        } else {
            if(Settings.currentFile !== "") {
                console.debug("BANG!");
            }
        }
        if(viewLoader.item) {
            if(fileName.indexOf("://") > 0) {
                viewLoader.item.source = fileName;
            } else {
                if(fileName[0] !== "/") {
                    fileName = "/" + fileName;
                }
                viewLoader.item.source = "file://" + fileName;
            }
        }
    }
    ToolManager {
        id: toolManager;
        onCurrentToolChanged: console.debug("Current tool is now " + currentTool.toolId());
    }
    Loader {
        id: viewLoader;
        anchors {
            top: toolbar.top;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
    }
    Connections {
        target: viewLoader.item;
        // ok, so this looks weird, but we explicitly want to keep the notes
        // panel open unless they're interacting with the other gui bits, not
        // if they're moving through the document
        onCanvasInteractionStarted: closeToolbarMenus(notesButton);
    }
    Component { id: stageView; StageDocumentPage {} }
    Component { id: wordsView; WordsDocumentPage {} }
    function closeToolbarMenus(sender) {
        if(sender !== textStyleButton) { textStyleButton.checked = false; }
        if(sender !== imageToolsButton) { imageToolsButton.checked = false; }
        if(sender !== optionsButton1) { optionsButton1.checked = false; }
        if(sender !== notesButton) { notesButton.checked = false; }
        if(sender !== optionsButton2) { optionsButton2.checked = false; }
    }
    Item {
        id: toolbar;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
        }
        opacity: viewLoader.item ? viewLoader.item.toolbarOpacity : 1;
        Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
        height: Settings.theme.adjustedPixel(86);
        MouseArea {
            anchors.fill: parent;
            // same logic as on canvas interaction - close menus, leave notes menu open
            onClicked: closeToolbarMenus(notesButton);
        }
        Rectangle {
            anchors.fill: parent;
            color: Settings.theme.color("components/toolbar/base");
            opacity: 0.96;
        }
        Rectangle {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            height: 1;
            color: "black";
            opacity: 0.1;
        }
        Item {
            id: appButton;
            height: parent.height;
            width: Settings.theme.adjustedPixel(110);
            Rectangle {
                anchors.fill: parent;
                color: "#f2b200";
            }
            Image {
                anchors {
                    left: parent.left;
                    verticalCenter: parent.verticalCenter;
                }
                height: Settings.theme.adjustedPixel(32);
                width: height;
                source: Settings.theme.icon("Arrow-Back-FileBrowse-1");
                sourceSize.width: width > height ? height : width;
                sourceSize.height: width > height ? height : width;
            }
            Image {
                anchors {
                    right: parent.right;
                    rightMargin: Kirigami.Units.largeSpacing;
                    verticalCenter: parent.verticalCenter;
                }
                height: Settings.theme.adjustedPixel(86) - Kirigami.Units.largeSpacing * 2;
                width: height;
                source: Settings.theme.icon("Calligra-MockIcon-1");
                sourceSize.width: width > height ? height : width;
                sourceSize.height: width > height ? height : width;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    closeToolbarMenus();
                    if(viewLoader.item.canvas.document.document.isModified()) {
                        saveBeforeExitDialog.show();
                    }
                    else {
                        applicationWindow().pageStack.layers.pop();
                    }
                }
            }
        }
        Row {
            anchors {
                left: appButton.right;
                leftMargin: 20;
                verticalCenter: parent.verticalCenter;
            }
            height: Settings.theme.adjustedPixel(66);
            spacing: Settings.theme.adjustedPixel(8);
            CohereButton {
                anchors.verticalCenter: parent.verticalCenter;
                text: "SAVE";
                textColor: Settings.theme.color("components/toolbar/text");
                font: Settings.theme.font("toolbar");
                onClicked: {
                    closeToolbarMenus();
                    viewLoader.item.canvas.document.save();
                }
            }
            CohereButton {
                anchors.verticalCenter: parent.verticalCenter;
                text: "UNDO";
                textColor: Settings.theme.color("components/toolbar/text");
                font: Settings.theme.font("toolbar");
                enabled: typeof(undoaction) !== "undefined" ? undoaction.enabled : false;
                onClicked: {
                    closeToolbarMenus();
                    undoaction.trigger();
                }
            }
            Button {
                height: parent.height;
                width: height * 2;
                text: "REDO";
                textColor: Settings.theme.color("components/toolbar/text");
                font: Settings.theme.font("toolbar");
                enabled: typeof(redoaction) !== "undefined" ? redoaction.enabled : false;
                onClicked: {
                    closeToolbarMenus();
                    redoaction.trigger();
                }
            }
            Item {
                height: parent.height;
                width: Settings.theme.adjustedPixel(8);
            }
            CohereButton {
                anchors.verticalCenter: parent.verticalCenter;
                text: "DONE"
                textColor: Settings.theme.color("components/toolbar/text");
                font: Settings.theme.font("toolbar");
                visible: viewLoader.item ? !viewLoader.item.navigateMode : false;
                onClicked: {
                    closeToolbarMenus();
                    viewLoader.item.navigateMode = true;
                }
            }
        }
        Row {
            id: toolbarTextTool
            anchors.centerIn: parent;
            height: Settings.theme.adjustedPixel(54);
            spacing: Settings.theme.adjustedPixel(4);
            opacity: (toolManager.currentTool !== null && toolManager.currentTool.toolId() === "TextToolFactory_ID") ? 1 : 0;
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
            Button {
                id: textStyleButton;
                image: Settings.theme.icon("SVG-Icon-TextStyle-1");
                imageMargin: 2;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
                checkable: true; checkedMargin: 0;
                radius: 4;
                onClicked: closeToolbarMenus(textStyleButton);
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    anchors.top: parent.bottom;
                    height: Kirigami.Units.largeSpacing;
                    width: height;
                    color: "#e8e9ea";
                    rotation: 45;
                    opacity: parent.checked ? 1 : 0;
                    Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                }
                TextStylePanel {
                    id: textStylePanel;
                    document: viewLoader.item ? viewLoader.item.document : null;
                    textEditor: viewLoader.item ? viewLoader.item.textEditor : null;
                    zoomLevel: viewLoader.item && viewLoader.item.canvas && viewLoader.item.canvas.zoomAction ? viewLoader.item.canvas.zoomAction.effectiveZoom : 1;
                }
            }
            Item { height: parent.height; width: 1; }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); opacity: 0.3; height: parent.height; width: 1; }
            Item { height: parent.height; width: 1; }
            Label {
                text: textStylePanel.cursorFont.family;
                height: parent.height; width: Constants.GridWidth;
                color: Settings.theme.color("components/toolbar/text");
                font: Settings.theme.font("toolbar");
                horizontalAlignment: Text.AlignLeft;
            }
            Label {
                text: textStylePanel.cursorFont.pointSize;
                height: parent.height; width: Constants.GridWidth / 2;
                color: Settings.theme.color("components/toolbar/text");
                font: Settings.theme.font("toolbar");
                horizontalAlignment: Text.AlignRight
                verticalAlignment: Text.AlignVCenter;
            }
            Item { height: parent.height; width: 1; }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); opacity: 0.3; height: parent.height; width: 1; }
            Item { height: parent.height; width: 1; }
            Button {
                image: Settings.theme.icon("SVG-Icon-Bold-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
                radius: 4;
                checkedMargin: 0;
                checked: textStylePanel.cursorFont ? textStylePanel.cursorFont.bold : false;
                onClicked: closeToolbarMenus();
            }
            Button {
                image: Settings.theme.icon("SVG-Icon-Italic-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
                radius: 4;
                checkedMargin: 0;
                checked: textStylePanel.cursorFont ? textStylePanel.cursorFont.italic : false;
                onClicked: closeToolbarMenus();
            }
            Button {
                image: Settings.theme.icon("SVG-Icon-Underline-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
                radius: 4;
                checkedMargin: 0;
                checked: textStylePanel.cursorFont ? textStylePanel.cursorFont.underline : false;
                onClicked: closeToolbarMenus();
            }
            Item { height: parent.height; width: 1; }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); opacity: 0.3; height: parent.height; width: 1; }
            Item { height: parent.height; width: 1; }
            Button {
                image: Settings.theme.icon("SVG-Icon-BulletList-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
                onClicked: closeToolbarMenus();
            }
            Button {
                image: Settings.theme.icon("SVG-Icon-TextAlignment-1");
                imageMargin: 4;
                height: parent.height; width: height;
                textColor: Settings.theme.color("components/toolbar/text");
                onClicked: closeToolbarMenus();
            }
        }
        Row {
            id: toolbarImageTool
            anchors.centerIn: parent;
            height: Settings.theme.adjustedPixel(54);
            spacing: Settings.theme.adjustedPixel(4);
            opacity: (toolManager.currentTool !== null && toolManager.currentTool.toolId() === "InteractionTool") ? 1 : 0;
            Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
            Button {
                image: Settings.theme.icon("SVG-Icon-Image");
                imageMargin: 4;
                height: parent.height; width: height;
                onClicked: closeToolbarMenus();
            }
            Item { height: parent.height; width: 1; }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); opacity: 0.3; height: parent.height; width: 1; }
            Item { height: parent.height; width: 1; }
            Button {
                image: Settings.theme.icon("SVG-Icon-Pen");
                imageMargin: 4;
                height: parent.height; width: height;
                onClicked: closeToolbarMenus();
            }
            Button {
                image: Settings.theme.icon("SVG-Icon-Fill");
                imageMargin: 4;
                height: parent.height; width: height;
                onClicked: closeToolbarMenus();
            }
            Button {
                image: Settings.theme.icon("SVG-Icon-Lines");
                imageMargin: 4;
                height: parent.height; width: height;
                onClicked: closeToolbarMenus();
            }
            Button {
                image: Settings.theme.icon("SVG-Icon-Shadow");
                imageMargin: 4;
                height: parent.height; width: height;
                onClicked: closeToolbarMenus();
            }
            Item { height: parent.height; width: 1; }
            Rectangle { color: Settings.theme.color("components/toolbar/text"); opacity: 0.3; height: parent.height; width: 1; }
            Item { height: parent.height; width: 1; }
            Button {
                id: imageToolsButton;
                height: parent.height; width: height;
                image: Settings.theme.icon("SVG-Icon-ImageAdjustment-1");
                imageMargin: 4;
                checkable: true; checkedMargin: 0;
                radius: 4;
                onClicked: closeToolbarMenus(imageToolsButton);
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    anchors.top: parent.bottom;
                    height: Kirigami.Units.largeSpacing;
                    width: height;
                    color: "#22282f";
                    rotation: 45;
                    opacity: parent.checked ? 0.96 : 0;
                    Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                }
                ImageToolsPanel { canvas: viewLoader.item && viewLoader.item.canvas ? viewLoader.item.canvas : null; }
            }
        }
        Row {
            anchors {
                right: parent.right;
                verticalCenter: parent.verticalCenter;
                rightMargin: Settings.theme.adjustedPixel(10);
            }
            height: Settings.theme.adjustedPixel(66);
            spacing: Settings.theme.adjustedPixel(10);
            visible: notesPanel.canvas ? false : true;
//             Button {
//                 height: parent.height; width: height;
//                 image: Settings.theme.icon("SVG-Icon-AddShape-1");
//                 onClicked: closeToolbarMenus();
//             }
//             Button {
//                 height: parent.height; width: height;
//                 image: Settings.theme.icon("SVG-Icon-Animations-1");
//                 onClicked: closeToolbarMenus();
//             }
            Button {
                height: parent.height; width: height;
                image: Settings.theme.icon("SVG-Icon-PlayPresentation-1");
                onClicked: {
                    applicationWindow().pageStack.layers.push(presentationDJMode);
                    closeToolbarMenus();
                    if(mainWindow.lastScreen() > 0) {
                        // don't do the whole song and dance with presenting unless we've got a useful
                        // external screen to do the presentation on. For on-device presentations, we
                        // need desktop controls anyway, so heuristics say - presentation in touch need
                        // external screen.
                        mainWindow.fullScreen = true;
                        DocumentManager.doc().setPresentationMonitor(mainWindow.lastScreen());
                        DocumentManager.doc().setPresenterViewEnabled(false);
                        mainWindow.desktopKoView.startPresentationFromBeginning();
                    }
                }
                Calligra.ContentsModel {
                    id: presentationModel
                    document: viewLoader.item ? viewLoader.item.document : null;
                    thumbnailSize: Qt.size(base.width, base.height);
                }
                Component { id: presentationDJMode; PresentationDJMode { }}//canvas: viewLoader.item ? viewLoader.item.canvas : null; } }
            }
            Button {
                id: optionsButton1;
                height: parent.height;
                width: height;
                image: Settings.theme.icon("SVG-Icon-Options-1");
                checkable: true;
                radius: 4;
                checkedColor: "#00adf5";
                checkedOpacity: 0.6;
                onClicked: closeToolbarMenus(optionsButton1);
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    anchors.top: parent.bottom;
                    height: Kirigami.Units.largeSpacing;
                    width: height;
                    color: "#4e5359";
                    rotation: 45;
                    opacity: parent.checked ? 0.96 : 0;
                    Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                }
                OptionsPanel { onInteractionStarted: closeToolbarMenus(); }
            }
        }
        Row {
            anchors {
                right: parent.right;
                verticalCenter: parent.verticalCenter;
                rightMargin: Settings.theme.adjustedPixel(10);
            }
            height: Settings.theme.adjustedPixel(66);
            spacing: Settings.theme.adjustedPixel(10);
            visible: notesPanel.canvas ? true : false;
//             Button {
//                 height: parent.height; width: height;
//                 image: Settings.theme.icon("SVG-Icon-AddShape-1");
//                 onClicked: closeToolbarMenus();
//             }
            Button {
                id: notesButton;
                height: parent.height; width: height;
                image: Settings.theme.icon("SVG-Icon-AddNote-1");
                checkable: true;
                radius: 4;
                checkedColor: "#00adf5";
                checkedOpacity: 0.6;
                onClicked: closeToolbarMenus(notesButton);
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    anchors.top: parent.bottom;
                    height: Kirigami.Units.largeSpacing;
                    width: height;
                    color: "#e8e9ea";
                    rotation: 45;
                    opacity: parent.checked ? 1 : 0;
                    Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                }
                NotesPanel {
                    id: notesPanel;
                    anchors {
                        top: parent.bottom;
                        right: parent.right;
                        rightMargin: - (parent.width + 20);
                        topMargin: 4;
                    }
                    opacity: parent.checked ? 1 : 0;
                    canvas: viewLoader.item && viewLoader.item.canvas && viewLoader.item.canvas.notes ? viewLoader.item.canvas : null;
                }
            }
            Button {
                id: optionsButton2;
                height: parent.height;
                width: height;
                image: Settings.theme.icon("SVG-Icon-Options-1");
                checkable: true;
                radius: 4;
                checkedColor: "#00adf5";
                checkedOpacity: 0.6;
                onClicked: closeToolbarMenus(optionsButton2);
                OptionsPanel { onInteractionStarted: closeToolbarMenus(); }
                Rectangle {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    anchors.top: parent.bottom;
                    height: Kirigami.Units.largeSpacing + 2;
                    width: height;
                    color: "#4e5359";
                    rotation: 45;
                    opacity: parent.checked ? 0.96 : 0;
                    Behavior on opacity { NumberAnimation { duration: Kirigami.Units.shortDuration; } }
                }
            }
        }
    }
    Dialog {
        id: saveBeforeExitDialog;
        title: "Save?";
        message: "The document was modified. Would you like to save it before closing it?";
        buttons: [ "Save", "Discard", "Cancel" ]
        onButtonClicked: {
            if(button === 0) {
                viewLoader.item.canvas.document.save();
            }
            else if(button === 1) {
                viewLoader.item.canvas.document.setModified(false);
                applicationWindow().pageStack.layers.pop();
            }
        }
    }
}
