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
    property alias source: wordsCanvas.source;
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
        anchors.fill: parent;
        Calligra.CanvasControllerItem {
            id: controllerItem;
            canvas: wordsCanvas;
            flickable: controllerFlickable;
            onMovingFastChanged: {
                if(movingFast === true) {
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
        ListView {
            id: navigatorListView;
            anchors.fill: parent;
            clip: true;
            model: wordsCanvas.documentModel;
            MouseArea {
                id: listViewMouseArea;
                anchors.fill: parent;
                hoverEnabled: true;
            }
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
                    color: "white";
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
//                    onClicked: wordsCanvas.currentPageNumber = index + 1;
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
            color: "darkslategrey";
            opacity: 0.8;
        }
        Label {
            anchors.centerIn: parent;
            color: "white";
            text: (wordsCanvas.documentModel === null) ? 0 : wordsCanvas.currentPageNumber + " of " + wordsCanvas.documentModel.rowCount();
        }
    }
}
