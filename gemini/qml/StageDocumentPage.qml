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

import QtQuick 2.0
import "components"
import org.kde.calligra 1.0 as Calligra

Item {
    id: base;
    signal canvasInteractionStarted();
    property alias document: stageDocument.document;
    property alias textEditor: stageDocument.textEditor;
    property QtObject canvas: stageCanvas;
    property alias source: stageDocument.source;
    property alias navigateMode: controllerFlickable.enabled;
    property double toolbarOpacity: 1;
    Calligra.Document {
        id: stageDocument;
        onStatusChanged: {
            if(status == Calligra.DocumentStatus.Loading) {
                baseLoadingDialog.visible = true;
            }
            else if(status == Calligra.DocumentStatus.Loaded) {
                console.debug("doc and part: " + stageDocument.doc + " " + stageDocument.part);
                mainWindow.setDocAndPart(stageDocument.doc, stageDocument.part);
                baseLoadingDialog.hideMe();
//                 thumbnailSize = Qt.size(Settings.theme.adjustedPixel(280), Settings.theme.adjustedPixel(360));
            }
        }
    }
    onNavigateModeChanged: {
        if(navigateMode === true) {
            // This means we've just changed back from having edited stuff.
            // Consequently we want to deselect all selections. Tell the canvas about that.
            stageCanvas.deselectEverything();
            toolManager.requestToolChange("InteractionTool");
        }
    }
    Item {
        anchors {
            top: parent.top;
            left: stageNavigator.right;
            right: parent.right;
            bottom: parent.bottom;
            topMargin: Settings.theme.adjustedPixel(86);
        }
        clip: true;
        Rectangle {
            anchors.fill: parent;
            color: "#4e5359";
        }
        
        Item {
            anchors.centerIn: parent;
            
            width: Math.min(stageDocument.documentSize.width, parent.width);
            height: Math.min(stageDocument.documentSize.height, parent.height);
        
            Calligra.View {
                id: stageCanvas;
                anchors.fill: parent;
                document: stageDocument;
            }
//             Calligra.PresentationCanvas {
//                 id: stageCanvas;
//                 
//                 anchors.fill: parent;
// 
//                 onLoadingBegun: baseLoadingDialog.visible = true;
//                 onLoadingFinished: {
//                     console.debug("doc and part: " + doc() + " " + part());
//                     mainWindow.setDocAndPart(doc(), part());
//                     baseLoadingDialog.hideMe();
//                 }
//                 onCurrentSlideChanged: navigatorListView.positionViewAtIndex(currentSlide, ListView.Contain);
//             }

            Flickable {
                id: controllerFlickable;
                anchors {
                    top: parent.top;
                    left: parent.left;
                    right: parent.right;
                    bottom: enabled ? parent.bottom : parent.top;
                }

                boundsBehavior: stageDocument.documentSize.width < base.width ? Flickable.StopAtBounds : Flickable.DragAndOvershootBounds;

                Calligra.ViewController {
                    id: controllerItem;
                    view: stageCanvas;
                    flickable: controllerFlickable;
                    minimumZoomFitsWidth: true;
                    Calligra.LinkArea {
                        anchors.fill: parent;
                        document: stageDocument;
                        onClicked: console.debug("clicked somewhere without a link");
                        onLinkClicked: console.debug("clicked on the link: " + linkTarget);
                        controllerZoom: controllerItem.zoom;
                    }
                }

                PinchArea {
                    x: controllerFlickable.contentX;
                    y: controllerFlickable.contentY;
                    height: controllerFlickable.height;
                    width: controllerFlickable.width;

                    onPinchStarted: {
                        base.canvasInteractionStarted();
                    }
                    onPinchUpdated: {
                        var newCenter = mapToItem( controllerFlickable, pinch.center.x, pinch.center.y );
                        controllerItem.zoomAroundPoint(pinch.scale - pinch.previousScale, newCenter.x, newCenter.y );
                    }
                    onPinchFinished: {
                        controllerFlickable.returnToBounds();
                    }

                    MouseArea {
                        anchors.fill: parent;
                        onClicked: {
                            if(mouse.x < width / 6) {
                                if(stageDocument.currentIndex === 0) {
                                    stageDocument.currentIndex = stageCanvas.indexCount() - 1;
                                }
                                else {
                                    stageDocument.currentIndex = stageDocument.currentIndex - 1;
                                }
                            }
                            else if(mouse.x > width * 5 / 6) {
                                var currentIndex = stageDocument.currentIndex;
                                stageDocument.currentIndex = stageDocument.currentIndex + 1;
                                if(currentIndex === stageDocument.currentIndex) {
                                    stageDocument.currentIndex = 0;
                                }
                            }
                            base.canvasInteractionStarted();
                        }
                        onDoubleClicked: {
                            if(mouse.x < width / 6 || mouse.x > width * 5 / 6) {
                                // don't accept double-clicks in the navigation zone
                                return;
                            }
                            toolManager.requestToolChange("InteractionTool");
                            base.navigateMode = false;
                            base.canvasInteractionStarted();
                        }
                    }
                }
            }
        }
    }
    Item {
        id: stageNavigator;
        anchors {
            top: parent.top;
            left: parent.left;
            bottom: parent.bottom;
        }
        width: Settings.theme.adjustedPixel(240);
        Rectangle {
            anchors.fill: parent;
            color: "#4e5359";
        }
        Rectangle {
            anchors {
                top: parent.top;
                right: parent.right;
                bottom: parent.bottom;
            }
            width: 1;
            color: "black";
            opacity: 0.1;
        }
        ListView {
            id: navigatorListView;
            anchors {
                fill: parent;
                topMargin: Settings.theme.adjustedPixel(86);
            }
            model: Calligra.ContentsModel {
                    id: stageNavigatorModel;
                    document: stageDocument;
                    thumbnailSize: Qt.size(Settings.theme.adjustedPixel(104) * 2, (navigatorListView.width - Settings.theme.adjustedPixel(56)) * 2);
                }
            delegate: Item {
                height: Settings.theme.adjustedPixel(136);
                width: ListView.view.width;
                Rectangle {
                    anchors.fill: parent;
                    opacity: index === stageDocument.currentIndex ? 0.6 : 0;
                    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
                    color: "#00adf5";
                }
                Label {
                    anchors {
                        top: parent.top;
                        left: parent.left;
                        bottom: parent.bottom;
                    }
                    width: Settings.theme.adjustedPixel(40);
                    color: index === stageDocument.currentIndex ? "white" : "#c1cdd1";
                    text: index + 1;
                    horizontalAlignment: Text.AlignHCenter;
                    verticalAlignment: Text.AlignVCenter;
                }
                Calligra.ImageDataItem {
                    anchors {
                        top: parent.top;
                        right: parent.right;
                        bottom: parent.bottom;
                        margins: Settings.theme.adjustedPixel(16);
                    }
                    width: parent.width - Settings.theme.adjustedPixel(56);
                    data: model.thumbnail;
                }
                MouseArea {
                    anchors.fill: parent;
                    onClicked: {
                        stageDocument.currentIndex = index;
                        base.canvasInteractionStarted();
                    }
                }
            }
        }
    }
}
