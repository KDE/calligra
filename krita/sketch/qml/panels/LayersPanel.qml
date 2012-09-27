/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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
import org.krita.sketch 1.0

Panel {
    id: base;
    name: "Layers";
    panelColor: Constants.Theme.SecondaryColor;

    actions: [
        Button {
            id: addButton;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-add.svg"
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: fullViewStack.push(editLayerPage);
        },
        Button {
            id: editButton;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-edit.svg"
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: fullViewStack.push(editLayerPage);
        },
        Item {
            width: (Constants.GridWidth * 2) - Constants.DefaultMargin - (Constants.GridHeight * 3)
            height: Constants.GridHeight;
        },
        Button {
            id: removeButton;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-remove.svg"
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: undefined;
        }
    ]

    LayerModel {
        id: layerModel;
        // Notice - the model needs to know about the engine before the view, hence it is set first
        // This could be rectified, but for now know that the order here is important.
        engine: QMLEngine;
        view: sketchView.view;
    }

    peekContents: ListView {
        anchors.fill: parent;
        model: layerModel;
        delegate: Rectangle {
            width: parent.width - Constants.DefaultMargin;
            height: childrenRect.height;
            color: "transparent";
            clip: true;Rectangle {
                id: topSpacer;
                height: model.childCount == 0 ? 0 : Constants.DefaultMargin;
                color: "transparent";
            }
            Rectangle {
                id: layerBgRect
                anchors {
                    top: topSpacer.bottom;
                    left: parent.left;
                    right: parent.right;
                    leftMargin: 8 * model.depth;
                }
                height: Constants.DefaultFontSize + 2*Constants.DefaultMargin;
                radius: 8
                opacity: model.activeLayer ? 0.5 : 0.2;
                color: "white";
            }
            Rectangle {
                anchors.fill: layerBgRect
                color: "transparent";
                Rectangle {
                    id: layerThumbContainer;
                    anchors {
                        verticalCenter: parent.verticalCenter;
                        left: parent.left;
                    }
                    height: Constants.DefaultFontSize + 2*Constants.DefaultMargin;
                    width: height;
                    color: "transparent";
                    Image {
                        anchors.centerIn: parent;
                        cache: false;
                        source: model.icon;
                        smooth: true;
                        width: parent.width * 0.8;
                        height: parent.height * 0.8;
                        fillMode: Image.PreserveAspectFit;
                    }
                }
                Text {
                    id: layerNameLbl
                    anchors {
                        top: parent.top;
                        left: layerThumbContainer.right;
                        right: parent.right;
                    }
                    text: model.name;
                    color: "black";
                    font.pixelSize: Constants.DefaultFontSize;
                    elide: Text.ElideRight;
                }
                MouseArea {
                    anchors.fill: parent;
                    onClicked: layerModel.setActive(model.index);
                }
            }
            Rectangle {
                id: bottomSpacer;
                anchors.top: layerBgRect.bottom;
                height: Constants.DefaultMargin;
                color: "transparent";
            }
        }
    }

    fullContents: PageStack {
        id: fullViewStack
        anchors.fill: parent;
        initialPage: ListView {
            anchors.fill: parent;
            model: layerModel;
            delegate: Rectangle {
                width: parent.width - Constants.DefaultMargin;
                height: childrenRect.height;
                color: "transparent";
                clip: true;

                Rectangle {
                    id: topSpacer;
                    height: model.childCount == 0 ? 0 : Constants.DefaultMargin;
                    color: "transparent";
                }
                Rectangle {
                    id: layerBgRect
                    anchors {
                        top: topSpacer.bottom;
                        left: parent.left;
                        right: parent.right;
                        leftMargin: 8 * model.depth;
                    }
                    height: Constants.GridHeight;
                    radius: 8
                    opacity: model.activeLayer ? 0.5 : 0.2;
                    color: "white";
                }
                Rectangle {
                    anchors.fill: layerBgRect
                    color: "transparent";
                    Rectangle {
                        id: layerThumbContainer;
                        anchors {
                            verticalCenter: parent.verticalCenter;
                            left: parent.left;
                        }
                        height: Constants.GridHeight;
                        width: height;
                        color: "transparent";
                        Image {
                            anchors.centerIn: parent;
                            cache: false;
                            source: model.icon;
                            smooth: true;
                            width: parent.width * 0.8;
                            height: parent.height * 0.8;
                            fillMode: Image.PreserveAspectFit;
                        }
                    }
                    Text {
                        id: layerNameLbl
                        anchors {
                            top: parent.top;
                            left: layerThumbContainer.right;
                            right: parent.right;
                        }
                        text: model.name;
                        color: "black";
                        font.pixelSize: Constants.DefaultFontSize;
                        elide: Text.ElideRight;
                    }
                    Text {
                        anchors {
                            top: layerNameLbl.bottom;
                            right: parent.right;
                            rightMargin: Constants.DefaultMargin;
                        }
                        text: "Mode: " + model.compositeDetails + ", " + model.percentOpacity + "%";
                        font.pixelSize: Constants.SmallFontSize;
                    }
                    MouseArea {
                        anchors.fill: parent;
                        onClicked: layerModel.setActive(model.index);
                    }
                    Row {
                        anchors {
                            left: layerThumbContainer.right;
                            bottom: parent.bottom;
                        }
                        height: childrenRect.height;
                        Rectangle {
                            width: Constants.DefaultFontSize;
                            height: width;
                            color: model.visible ? "silver" : "gray";
                            Text {
                                anchors.centerIn: parent;
                                font.pixelSize: Constants.SmallFontSize;
                                color: model.visible ? "black" : "white";
                                text: "V"
                            }
                            MouseArea {
                                anchors.fill: parent;
                                onClicked: layerModel.setVisible(model.index, !model.visible);
                            }
                        }
                        Rectangle {
                            width: Constants.DefaultFontSize;
                            height: width;
                            color: model.locked ? "silver" : "gray";
                            Text {
                                anchors.centerIn: parent;
                                font.pixelSize: Constants.SmallFontSize;
                                color: model.locked ? "black" : "white";
                                text: "L"
                            }
                            MouseArea {
                                anchors.fill: parent;
                                onClicked: layerModel.setLocked(model.index, !model.locked);
                            }
                        }
                    }
                }
                Rectangle {
                    id: bottomSpacer;
                    anchors.top: layerBgRect.bottom;
                    height: Constants.DefaultMargin;
                    color: "transparent";
                }
            }
        }
    }

    dragDelegate: Component {
        Rectangle {
            color: Constants.Theme.SecondaryColor;
            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
            radius: 8

            Label {
                anchors.centerIn: parent;

                text: "Layers";
                color: "white";

                font.pixelSize: Constants.DefaultFontSize;
            }
        }
    }

    Component { id: editLayerPage; EditLayerPage { layersModel: layerModel; } }
}
