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
    }
    QtObject {
        id: d;
        function showThings() {
            navigatorSidebar.x = Constants.DefaultMargin;
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
            margins: Constants.DefaultMargin;
            topMargin: Constants.DefaultMargin + Constants.ToolbarHeight;
        }
        x: -width;
        Behavior on x { PropertyAnimation { duration: Constants.AnimationDuration; } }
        width: Constants.GridWidth;
        Rectangle {
            anchors {
                left: parent.right;
                leftMargin: -1;
                verticalCenter: parent.verticalCenter;
            }
            height: Constants.GridHeight * 2;
            width: Constants.DefaultMargin * 3;
            color: "darkslategrey";
            opacity: 0.8;
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
                onClicked: d.showThings();
            }
        }
        Rectangle {
            anchors.fill: parent;
            radius: Constants.DefaultMargin;
            color: "darkslategrey";
            opacity: 0.8;
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
        }
        ListView {
            id: navigatorListView;
            anchors {
                fill: parent;
                topMargin: Constants.GridHeight / 2;
                bottomMargin: Constants.GridHeight / 2;
            }
            clip: true;
            model: wordsCanvas.documentModel;
            delegate: Item {
                width: Constants.GridWidth;
                height: width;
                Label {
                    anchors {
                        left: parent.left;
                        leftMargin: Constants.DefaultMargin;
                        verticalCenter: parent.verticalCenter;
                    }
                    text: index + 1;
                    font.pixelSize: Constants.SmallFontSize;
                    color: "#c1cdd1";
                }
                Image {
                    anchors {
                        top: parent.top;
                        right: parent.right;
                        bottom: parent.bottom;
                        margins: Constants.DefaultMargin;
                    }
                    width: parent.width * 3 / 4;
                    fillMode: Image.PreserveAspectFit;
                    source: model.decoration;
                }
                Rectangle {
                    anchors.fill: parent;
                    color: "cyan"
                    opacity: (wordsCanvas.currentPageNumber === index + 1) ? 0.3 : 0;
                    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
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
