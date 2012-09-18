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
import org.krita.sketch 1.0
import "../components"

Panel {
    id: base;
    name: "Tool";
    panelColor: "#0078B4";

    ToolManager {
        id: toolManager;
        view: sketchView.view;
    }

    CompositeOpModel {
        id: compositeOpModel;
        view: sketchView.view;
    }

    ListModel {
        id: paintingTools;
        ListElement {
            text: "Paint"
            name: "paint"
        }
        ListElement {
            text: "Fill"
            name: "fill"
        }
        ListElement {
            text: "Gradient"
            name: "gradient"
        }
    }

    ListModel {
        id: adjustmentTools;
        ListElement {
            text: "Move"
            name: "move"
        }
        ListElement {
            text: "Transform"
            name: "transform"
        }
        ListElement {
            text: "Crop"
            name: "crop"
        }
    }

    function toolNameToID(toolName) {
        var names = {
            "paint": "KritaShape/KisToolBrush",
            "fill": "KritaFill/KisToolFill",
            "gradient": "KritaFill/KisToolGradient",
            "move": "KritaTransform/KisToolMove",
            "transform": "KisToolTransform",
            "crop": "KisToolCrop"
        };
        return names[toolName];
    }

    function changeTool(toolName) {
        toolOptionsPeek.source = "toolconfigpages/" + toolName + ".qml";
        toolOptionsFull.source = "toolconfigpages/" + toolName + ".qml";
        toolManager.requestToolChange(toolNameToID(toolName));
    }

    actions: [
        Button {
            id: firstTool;
            property string toolName: "paint";
            onToolNameChanged: changeTool(toolName);
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-" + toolName + ".svg"
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: {
                fullContentsItem.state = "";
                changeTool(toolName);
            }
        },
        Button {
            id: secondTool;
            property string toolName: "move";
            onToolNameChanged: changeTool(toolName);
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-" + toolName + ".svg"
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: {
                fullContentsItem.state = "secondTool";
                changeTool(toolName);
            }
        },
        Item {
            width: (Constants.GridWidth * 2) - Constants.DefaultMargin - (Constants.GridHeight * 3)
            height: Constants.GridHeight;
        },
        Button {
            id: eraserButton;
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: "../images/svg/icon-erase.svg";
            textColor: "white";
            shadow: false;
            highlight: false;
            checked: compositeOpModel.eraserMode;
            onClicked: compositeOpModel.eraserMode = !compositeOpModel.eraserMode;
        }
    ]

    peekContents: Flickable {
        anchors.fill: parent;
        Loader {
            id: toolOptionsPeek;
            width: parent.width;
            onSourceChanged: item.fullView = false;
            source: "toolconfigpages/paint.qml";
        }
    }

    fullContents: Item {
        id: fullContentsItem;
        states: [
            State {
                name: "secondTool";
                PropertyChanges { target: firstToolSelector; visible: false; }
                PropertyChanges { target: secondToolSelector; visible: true; }
            }
        ]
        anchors.fill: parent;
        Item {
            id: toolSelectorContainer;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
            }
            height: childrenRect.height;
            Item {
                id: firstToolSelector;
                width: parent.width;
                height: Constants.GridHeight;
                Rectangle {
                    anchors.fill: parent;
                    opacity: 0.5;
                }
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    width: childrenRect.width;
                    spacing: Constants.DefaultMargin;
                    Repeater {
                        model: paintingTools;
                        delegate: Button {
                            width: height;
                            height: Constants.GridHeight
                            color: "transparent";
                            image: "../images/svg/icon-" + model.name + "-blue.svg"
                            textColor: "white";
                            shadow: false;
                            highlight: false;
                            onClicked: firstTool.toolName = model.name;
                        }
                    }
                }
            }
            Item {
                id: secondToolSelector;
                width: parent.width;
                height: Constants.GridHeight;
                visible: false;
                Rectangle {
                    anchors.fill: parent;
                    opacity: 0.5;
                }
                Row {
                    anchors.horizontalCenter: parent.horizontalCenter;
                    width: childrenRect.width;
                    spacing: Constants.DefaultMargin;
                    Repeater {
                        model: adjustmentTools;
                        delegate: Button {
                            width: height;
                            height: Constants.GridHeight
                            color: "transparent";
                            image: "../images/svg/icon-" + model.name + "-blue.svg"
                            textColor: "white";
                            shadow: false;
                            highlight: false;
                            onClicked: secondTool.toolName = model.name;
                        }
                    }
                }
            }
        }
        Flickable {
            anchors {
                top: toolSelectorContainer.bottom;
                left: parent.left;
                right: parent.right;
                bottom: parent.bottom;
            }
            Loader {
                id: toolOptionsFull;
                width: parent.width;
                source: "toolconfigpages/paint.qml";
            }
        }
    }

    dragDelegate: Component {
        Rectangle {
            color: "#0078B4";
            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
            radius: 8;

            Label {
                anchors.centerIn: parent;

                text: "Tool";
                color: "white";

                font.pixelSize: Constants.DefaultFontSize;
            }
        }
    }
}
