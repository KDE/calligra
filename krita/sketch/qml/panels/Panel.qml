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
import org.krita.draganddrop 1.0 as DnD
import "../components"

Item {
    id: base;

    property color panelColor: Constants.Theme.MainColor;
    property color textColor: "white";
    property string name;

    property alias actions: actionsLayout.children;
    property alias peekContents: peek.children;
    property alias fullContents: full.children;

    property real editWidth: Constants.GridWidth * 4;

    property Component dragDelegate;

    signal collapsed();
    signal peek();
    signal dragStarted();
    signal drop(int action);

    state: "collapsed";

    DropShadow {
        id: fill;

        width: parent.width;
        height: parent.height;

        Rectangle {
            id: rectangle3
            anchors.fill: parent;
            color: base.panelColor;
            clip: true;

            Rectangle {
                id: rectangle4
                anchors.fill: parent;
                color: "#ffffff"
                opacity: 0.630
            }

            Item {
                id: peek;

                opacity: 0;

                width: (Constants.GridWidth * 2) - 8;
                anchors.bottom: header.top
                height: Constants.GridHeight * 3;
            }

            Item {
                id: full;

                anchors.top: header.bottom;
                anchors.bottom: footer.top;

                width: parent.width;
            }

            DropShadow {
                id: header;

                anchors.left: parent.left;
                anchors.top: parent.top;
                anchors.right: parent.right;
                //y: Constants.GridHeight * 3;
                //width: parent.width;
                height: Constants.GridHeight;


                Rectangle {
                    id: rectanglehead
                    color: base.panelColor;
                    y: Constants.GridHeight / 2;
                    width: parent.width;
                    height: Constants.GridHeight / 2;
                    visible: false;
                }

                Rectangle {
                    id: rectangle1
                    anchors.fill: parent;
                    color: base.panelColor;

                    Flow {
                        id: actionsLayout;
                    }
                }
            }

            DropShadow {
                id: footer;

                anchors.bottom: parent.bottom;
                width: parent.width;
                height: Constants.GridHeight;

                Rectangle {
                    id: rectanglefoot
                    color: base.panelColor;
                    width: parent.width;
                    height: Constants.GridHeight / 2;
                }

                Rectangle {
                    anchors.fill: parent;
                    color: base.panelColor;

                    Label {
                        anchors.left: parent.left;
                        anchors.leftMargin: Constants.DefaultMargin;
                        anchors.verticalCenter: parent.verticalCenter;

                        text: base.name;
                        color: base.textColor;
                    }
                }

                DnD.DragArea {
                    anchors.fill: parent;
                    delegate: base.dragDelegate;
                    source: base;

                    onDragStarted: base.dragStarted();
                    onDrop: base.drop(action);
                }
            }
        }
    }

    Rectangle {
        id: handle;

        anchors.top: fill.bottom;

        color: base.panelColor;
        radius: 0

        width: (Constants.GridWidth * 1) - 8
        height: Constants.GridHeight / 4

        opacity: 0


        Rectangle {
            id: rectangle2
            anchors.top: parent.top;

            width: (Constants.GridWidth * 1) - 8
            height: Constants.GridHeight / 2
            color: base.panelColor
            radius: 8
            clip: true
        }

        Label {
            id: handleLabel;


            text: base.name;
            anchors.horizontalCenter: rectangle2.horizontalCenter
            anchors.verticalCenter: rectangle2.verticalCenter
            color: base.textColor;

            font.pixelSize: Constants.DefaultFontSize;
        }

        DnD.DragArea {
            anchors.top: rectangle2.bottom
            anchors.right: rectangle2.left
            anchors.bottom: rectangle2.top
            anchors.left: rectangle2.right
            delegate: base.dragDelegate;
            source: base;

            onDragStarted: base.dragStarted();
            onDrop: base.drop(action);

            MouseArea {
                anchors.fill: parent;
                onClicked: base.state = base.state == "peek" ? "collapsed" : "peek";
            }
        }
    }

    states: [
        State {
            name: "collapsed";

            PropertyChanges { target: base; width: Constants.GridWidth; }
            PropertyChanges { target: fill; opacity: 0; height: 0; }
            PropertyChanges { target: handle; opacity: 1; }
        },
        State {
            name: "peek";

            PropertyChanges { target: base; width: Constants.GridWidth * 3; }
            PropertyChanges { target: fill; height: Constants.GridHeight * 2; }
            PropertyChanges { target: header; height: Constants.GridHeight * 2; width: Constants.GridWidth; }
            PropertyChanges { target: handle; opacity: 1; }
            PropertyChanges { target: peek; opacity: 1; }
            PropertyChanges { target: full; opacity: 0; }
            PropertyChanges { target: footer; opacity: 0; }
        },
        State {
            name: "full";
        },
        State {
            name: "edit";

            PropertyChanges { target: base; width: base.editWidth; }
        }
    ]

    transitions: [
        Transition {
            from: "collapsed";
            to: "peek";

            SequentialAnimation {
                ScriptAction { script: base.peek(); }
                PropertyAction { targets: [ header, footer ]; properties: "height,width,opacity" }
                NumberAnimation { targets: [ base, fill, handle, peek, full ]; properties: "height,width,opacity"; duration: 250; }
            }
        },
        Transition {
            from: "peek";
            to: "collapsed";

            SequentialAnimation {
                NumberAnimation { targets: [ base, fill, handle, peek, full ]; properties: "height,width,opacity"; duration: 250; }
                PropertyAction { targets: [ header, footer ]; properties: "height,width,opacity" }
                ScriptAction { script: base.collapsed(); }
            }
        },
        Transition {
            from: "peek";
            to: "full";
            reversible: true;

            NumberAnimation { properties: "height,width,opacity"; duration: 250; }
        },
        Transition {
            from: "collapsed";
            to: "full";

            NumberAnimation { properties: "height,width,opacity"; duration: 250; }
        },
        Transition {
            from: "full";
            to: "collapsed";

            NumberAnimation { properties: "height,width,opacity"; duration: 250; }
        },
        Transition {
            from: "full"
            to: "edit"
            reversible: true;

            NumberAnimation { properties: "width"; duration: 250; }
        }
    ]
}