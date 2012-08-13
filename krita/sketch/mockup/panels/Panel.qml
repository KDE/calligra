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

Item {
    id: base;

    property color panelColor: Constants.Theme.MainColor;
    property color textColor: "white";
    property string name;

    property alias actions: actionsLayout.children;
    property alias peekContents: peek.children;
    property alias fullContents: full.children;

    property real editWidth: Constants.GridWidth * 4;

    signal collapsed();
    signal expanded();

    state: "collapsed";

    DropShadow {
        x: handle.x;
        y: handle.y;
        width: handle.width;
        height: handle.height;
        visible: handle.visible;
    }

    DropShadow {
        id: fill;

        width: parent.width;
        height: parent.height;

        Rectangle {
            anchors.fill: parent;
            color: "white";

            clip: true;

            Item {
                id: peek;

                opacity: 0;

                width: Constants.GridWidth * 2;
                height: Constants.GridHeight * 2;
            }

            Item {
                id: full;

                anchors.top: header.bottom;
                anchors.bottom: footer.top;

                width: parent.width;
            }

            DropShadow {
                id: header;

                anchors.top: parent.top;
                anchors.right: parent.right;

                width: parent.width;
                height: Constants.GridHeight;

                Rectangle {
                    anchors.fill: parent;
                    color: base.panelColor;

                    Flow {
                        id: actionsLayout;
                        anchors.fill: parent;
                    }
                }
            }

            DropShadow {
                id: footer;

                anchors.bottom: parent.bottom;
                width: parent.width;
                height: Constants.GridHeight;

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

                MouseArea {
                    anchors.fill: parent;

                    property real start: NaN;

                    onPressed: start = mouse.y;
                    onPositionChanged: {
                        var dist = mouse.y - start;
                        if( dist < -30 ) {
                            base.state = "collapsed";
                        }
                    }
                }
            }
        }
    }

    Rectangle {
        id: handle;

        anchors.top: fill.bottom;

        color: base.panelColor;

        width: Constants.GridWidth;
        height: Constants.GridHeight / 2;

        opacity: 0;

        Label {
            id: handleLabel;

            anchors.centerIn: parent;

            text: base.name;
            color: base.textColor;

            font.pixelSize: Constants.SmallFontSize;
        }

        MouseArea {
            anchors.fill: parent;

            property real start: NaN;

            onClicked: base.state = base.state == "peek" ? "collapsed" : "peek";
            onPressed: start = mouse.y;
            onPositionChanged: {
                var dist = mouse.y - start;
                if( dist > 30 )
                    base.state = "full";
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
            reversible: true;

            SequentialAnimation {
                PropertyAction { targets: [ header, footer ]; properties: "height,width,opacity" }
                NumberAnimation { targets: [ base, fill, handle, peek, full ]; properties: "height,width,opacity"; duration: 250; }
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

            SequentialAnimation {
                NumberAnimation { properties: "height,width,opacity"; duration: 250; }
                ScriptAction { script: base.expanded(); }
            }
        },
        Transition {
            from: "full";
            to: "collapsed";

            SequentialAnimation {
                NumberAnimation { properties: "height,width,opacity"; duration: 250; }
                ScriptAction { script: base.collapsed(); }
            }
        },
        Transition {
            from: "full"
            to: "edit"
            reversible: true;

            NumberAnimation { properties: "width"; duration: 250; }
        }
    ]
}
