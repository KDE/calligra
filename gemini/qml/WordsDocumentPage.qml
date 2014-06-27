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
import "components"
import org.calligra.CalligraComponents 0.1 as Calligra

Item {
    id: base;
    property alias document: wordsCanvas.document;
    property alias textEditor: wordsCanvas.textEditor;
    property QtObject canvas: wordsCanvas;
    property alias source: wordsCanvas.source;
    property alias navigateMode: controllerFlickable.enabled;
    onNavigateModeChanged: {
        if(navigateMode === true) {
            // This means we've just changed back from having edited stuff.
            // Consequently we want to deselect all selections. Tell the canvas about that.
            wordsCanvas.deselectEverything();
            toolManager.requestToolChange("PageToolFactory_ID");
        }
    }
    function scrollToEnd() {
        controllerFlickable.contentY = controllerFlickable.contentHeight - controllerFlickable.height;
    }
    Calligra.TextDocumentCanvas {
        id: wordsCanvas;
        anchors.fill: parent;
        onLoadingBegun: baseLoadingDialog.visible = true;
        onLoadingFinished: {
            console.debug("doc and part: " + doc() + " " + part());
            mainWindow.setDocAndPart(doc(), part());
            baseLoadingDialog.hideMe();
        }
        onCurrentPageNumberChanged: navigatorListView.positionViewAtIndex(currentPageNumber - 1, ListView.Contain);
    }
    Flickable {
        id: controllerFlickable;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            bottom: enabled ? parent.bottom : parent.top;
        }
        Image {
            height: Settings.theme.adjustedPixel(40);
            width: Settings.theme.adjustedPixel(40);
            source: Settings.theme.icon("intel-Words-Handle-cursor");
            opacity: wordsCanvas.hasSelection ? 1 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            x: wordsCanvas.selectionStartPos.x - width / 2;
            y: wordsCanvas.selectionStartPos.y - (height - 4);
            Rectangle {
                anchors {
                    top: parent.bottom;
                    horizontalCenter: parent.horizontalCenter;
                }
                height: wordsCanvas.selectionStartPos.height - 4;
                width: 4;
                color: "#009bcd";
            }
        }
        Image {
            height: Settings.theme.adjustedPixel(40);
            width: Settings.theme.adjustedPixel(40);
            source: Settings.theme.icon("intel-Words-Handle-cursor");
            opacity: wordsCanvas.hasSelection ? 1 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            x: wordsCanvas.selectionEndPos.x - width / 2;
            y: wordsCanvas.selectionEndPos.y + (wordsCanvas.selectionEndPos.height - 4);
            Rectangle {
                anchors {
                    bottom: parent.top;
                    horizontalCenter: parent.horizontalCenter;
                }
                height: wordsCanvas.selectionEndPos.height - 4;
                width: 4;
                color: "#009bcd";
            }
        }
        MouseArea {
            x: controllerFlickable.contentX;
            y: controllerFlickable.contentY;
            height: controllerFlickable.height;
            width: controllerFlickable.width;
            onClicked: {
                if(mouse.x < width / 6) {
                    controllerItem.pageChanging = true;
                    controllerFlickable.contentY = Math.max(0, controllerFlickable.contentY - controllerFlickable.height + (Constants.GridHeight * 1.5));
                    controllerItem.pageChanging = false;
                }
                else if(mouse.x > width * 5 / 6) {
                    controllerItem.pageChanging = true;
                    controllerFlickable.contentY = Math.min(controllerFlickable.contentHeight - controllerFlickable.height, controllerFlickable.contentY + controllerFlickable.height - (Constants.GridHeight * 1.5));
                    controllerItem.pageChanging = false;
                }
            }
            onDoubleClicked: {
                if(mouse.x < width / 6 || mouse.x > width * 5 / 6) {
                    // don't accept double-clicks in the navigation zone
                    return;
                }
                toolManager.requestToolChange("TextToolFactory_ID");
                base.navigateMode = false;
            }
        }
        Calligra.CanvasControllerItem {
            id: controllerItem;
            canvas: wordsCanvas;
            flickable: controllerFlickable;
            property bool pageChanging: false;
            onMovingFastChanged: {
                if(movingFast === true && !pageChanging) {
                    d.showThings();
                }
                else {
                    d.hideThings();
                }
            }
        }
    }
    QtObject {
        id: d;
        function showThings() {
            navigatorSidebar.x = 0;
            pageNumber.opacity = 1;
            hideTimer.stop();
        }
        function hideThings() {
            if(navigatorSidebar.containsMouse) {
                return;
            }
            hideTimer.start();
        }
    }
    Timer {
        id: hideTimer;
        running: false;
        repeat: false;
        interval: 2000;
        onTriggered: {
            navigatorSidebar.x = -navigatorSidebar.width;
            pageNumber.opacity = 0;
        }
    }
    Item {
        id: navigatorSidebar;
        property alias containsMouse: listViewMouseArea.containsMouse;
        anchors {
            top: parent.top;
            bottom: parent.bottom;
            topMargin: Settings.theme.adjustedPixel(40) + Constants.ToolbarHeight;
            bottomMargin: Settings.theme.adjustedPixel(40);
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
            opacity: (parent.x > -parent.width) ? 1 : 0;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
        }
        x: -width;
        Behavior on x { PropertyAnimation { duration: Constants.AnimationDuration; } }
        width: Settings.theme.adjustedPixel(190);
        Item {
            anchors {
                left: parent.right;
                verticalCenter: parent.verticalCenter;
            }
            height: Constants.GridHeight * 2;
            width: Constants.DefaultMargin * 3;
            clip: true;
            Rectangle {
                anchors {
                    fill: parent;
                    leftMargin: -(radius + 1);
                }
                radius: Constants.DefaultMargin;
                color: "#55595e";
                opacity: 0.5;
            }
            Rectangle {
                anchors {
                    top: parent.top;
                    bottom: parent.bottom;
                    margins: Constants.DefaultMargin;
                    horizontalCenter: parent.horizontalCenter;
                }
                width: 4;
                radius: 2;
                color: "white";
                opacity: 0.5;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    if(navigatorSidebar.x === 0) {
                        navigatorSidebar.x = -navigatorSidebar.width;
                        pageNumber.opacity = 0;
                    }
                    else {
                        d.showThings();
                    }
                }
            }
        }
        Rectangle {
            anchors {
                fill: parent;
                leftMargin: -Constants.DefaultMargin + 1;
            }
            radius: Constants.DefaultMargin;
            color: "#55595e";
            opacity: 0.5;
            Rectangle {
                anchors.fill: parent;
                radius: parent.radius;
                color: "transparent";
                border.width: 1;
                border.color: "black";
                opacity: 0.6;
            }
        }
        MouseArea {
            id: listViewMouseArea;
            anchors.fill: parent;
            hoverEnabled: true;
        }
        Button {
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: Constants.GridHeight / 2;
            image: Settings.theme.icon("Arrow-ScrollUp-1");
            imageMargin: 2;
            Rectangle {
                anchors {
                    left: parent.left;
                    right: parent.right;
                    rightMargin: 1;
                    bottom: parent.bottom;
                }
                height: 1;
                color: "black";
                opacity: 0.3;
            }
        }
        Button {
            anchors {
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            height: Constants.GridHeight / 2;
            image: Settings.theme.icon("Arrow-ScrollDown-1");
            imageMargin: 2;
            Rectangle {
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    rightMargin: 1;
                }
                height: 1;
                color: "black";
                opacity: 0.3;
            }
        }
        ListView {
            id: navigatorListView;
            anchors {
                fill: parent;
                topMargin: Constants.GridHeight / 2;
                bottomMargin: Constants.GridHeight / 2;
                rightMargin: 1;
            }
            clip: true;
            model: wordsCanvas.documentModel;
            delegate: Item {
                width: Settings.theme.adjustedPixel(190);
                height: Settings.theme.adjustedPixel(190);
                Image {
                    anchors {
                        top: parent.top;
                        right: parent.right;
                        bottom: parent.bottom;
                        margins: Settings.theme.adjustedPixel(5);
                    }
                    width: Settings.theme.adjustedPixel(140);
                    fillMode: Image.PreserveAspectFit;
                    source: model.decoration;
                    Rectangle {
                        anchors.fill: parent;
                        color: "transparent";
                        border.width: 1;
                        border.color: "black";
                        opacity: 0.1;
                    }
                }
                Rectangle {
                    anchors.fill: parent;
                    color: "#00adf5"
                    opacity: (wordsCanvas.currentPageNumber === index + 1) ? 0.4 : 0;
                    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                }
                Label {
                    anchors {
                        left: parent.left;
                        leftMargin: Constants.DefaultMargin;
                        verticalCenter: parent.verticalCenter;
                    }
                    text: index + 1;
                    font: Settings.theme.font("applicationSemi");
                    color: "#c1cdd1";
                }
                MouseArea {
                    anchors.fill: parent;
                    onClicked: controllerFlickable.contentY = wordsCanvas.pagePosition(index + 1);
                }
            }
        }
    }
    Item {
        id: pageNumber;
        anchors {
            right: parent.right;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
        }
        opacity: 0;
        Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
        height: Constants.GridHeight / 2;
        width: Constants.GridWidth;
        Rectangle {
            anchors.fill: parent;
            radius: Constants.DefaultMargin;
            color: Settings.theme.color("components/overlay/base");
            opacity: 0.7;
        }
        Label {
            anchors.centerIn: parent;
            color: Settings.theme.color("components/overlay/text");
            text: (wordsCanvas.documentModel === null) ? 0 : wordsCanvas.currentPageNumber + " of " + wordsCanvas.documentModel.rowCount();
        }
    }
}
