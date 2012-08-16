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
        id: fill;

        width: parent.width;
        height: parent.height;
        size: 0

        Rectangle {
            id: rectangle3
            anchors.fill: parent;
            color: "white";

            clip: true;

            Rectangle {
                id: rectangle4
                anchors.top: parent.top;
                anchors.left: parent.left;
                width: parent.width;
                height: parent.height;
                color: "#ffffff"
                opacity: 0
            }

            Item {
                id: peek;

                opacity: 0;

                width: (Constants.GridWidth * 2) - 8;
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

                anchors.bottom: parent.bottom;
                anchors.left: parent.left;

                width: parent.width;
                height: Constants.GridHeight;

                Rectangle {
                    id: rectangle1
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
                    id: rectangle5
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
        radius: 0

        width: (Constants.GridWidth * 1) - 8
        height: Constants.GridHeight / 4

        opacity: 0


        Rectangle {
            id: rectangle2

            anchors.top: fill.bottom;

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

        MouseArea {
            id: mousearea1

            property real start: NaN;
            anchors.rightMargin: -120
            anchors.leftMargin: -120
            anchors.bottomMargin: -16
            anchors.top: rectangle2.bottom
            anchors.right: rectangle2.left
            anchors.bottom: rectangle2.top
            anchors.left: rectangle2.right
            anchors.topMargin: -32

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

            PropertyChanges { target: base; width: (Constants.GridWidth * 1) - 8 }
            PropertyChanges { target: fill; opacity: 0; height: 0; }
            PropertyChanges { target: handle; x: 0; y: 0; width: (Constants.GridWidth * 1) - 8; anchors.topMargin: 0; opacity: 1; }

            PropertyChanges {
                target: rectangle1
                x: 0
                y: 0
                width: 120
                height: 64
                anchors.leftMargin: 0
                anchors.topMargin: 0
            }

            PropertyChanges {
                target: mousearea1
                x: 0
                y: 0
                width: 120
                height: 32
                anchors.topMargin: 0
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
            }

            PropertyChanges {
                target: header
                x: 0
                y: 0
                anchors.topMargin: 0
            }

            PropertyChanges {
                target: actionsLayout
                x: 0
                y: 0
                width: 120
                height: 64
                anchors.topMargin: 0
            }

            PropertyChanges {
                target: footer
                anchors.bottomMargin: 0
            }

            PropertyChanges {
                target: item1
                x: 0
                y: 0
                width: 120
                height: 32
                opacity: 1
            }

            PropertyChanges {
                target: rectangle2
                x: 0
                y: 0
                color: base.panelColor
                opacity: 1
            }
        },
        State {
            name: "peek";

            PropertyChanges { target: base; width: (Constants.GridWidth * 2) - 8 }
            PropertyChanges { target: fill; height: Constants.GridHeight * 4 }
            PropertyChanges { target: header; height: Constants.GridHeight; width: (Constants.GridWidth * 2) - 8 }
            PropertyChanges { target: handle; width: (Constants.GridWidth * 1)-8; opacity: 1; }
            PropertyChanges { target: peek; opacity: 1; }
            PropertyChanges { target: full; opacity: 0; }
            PropertyChanges { target: footer; opacity: 0; }

            PropertyChanges {
                target: rectangle3
                x: 0
                y: 0
                color: base.panelColor
                anchors.topMargin: 0
                anchors.rightMargin: 0
                anchors.bottomMargin: 0
                anchors.leftMargin: 0
            }

            PropertyChanges {
                target: rectangle4
                opacity: 0.630
            }
        },
        State {
            name: "full";

            PropertyChanges {
                target: rectangle3
                radius: 8
            }

            PropertyChanges {
                target: rectangle5
                radius: 0
                clip: false
            }

            PropertyChanges {
                target: rectangle6
                radius: 8
                clip: true
            }
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
