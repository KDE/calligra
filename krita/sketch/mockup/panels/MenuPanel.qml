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

    property bool collapsed: true;

    property alias newButtonChecked: newButton.checked;
    property alias openButtonChecked: openButton.checked;

    signal buttonClicked( string button );

    height: Constants.GridHeight;

    Rectangle {
        id: background;
        color: "#1d3458"
        anchors.fill: parent;

        Row {
            Button {
                id: newButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/document-new.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "new" );
            }
            Button {
                id: openButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/document-open.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "open" );
            }
            Button {
                id: saveButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/document-save.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "save" );
            }
            Button {
                id: saveAsButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/document-save-as.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "saveAs" );
            }
            Button {
                id: shareButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/document-share.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "share" );
            }
        }

        Row {
            anchors.right: parent.right;

            Button {
                id: undoButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/edit-undo.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "undo" );
            }
            Button {
                id: redoButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/edit-redo.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "redo" );
            }
            Item {
                width: Constants.GridWidth;
                height: Constants.GridHeight;
            }
            Button {
                id: helpButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/help-about.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "help" );
            }
            Button {
                id: settingsButton;
                width: Constants.GridWidth;
                height: Constants.GridHeight;
                image: ":/images/configure.png";
                highlightColor: Constants.Theme.HighlightColor;
                onClicked: base.buttonClicked( "settings" );
            }
        }
    }

    Item {
        anchors.bottom: parent.top;
        anchors.horizontalCenter: parent.horizontalCenter;

        width: Constants.GridWidth * 2
        height: Constants.GridHeight /2

        Rectangle {
            id: rectangle1
            x: 8
            y: 8
            width: 240
            height: 24
            color: "#1d3458"
            radius: 0
            anchors.topMargin: 8
            anchors.leftMargin: 8
            anchors.rightMargin: 8
            anchors.bottomMargin: 0
            anchors.fill: parent;

            BorderImage {
                id: border_image5
                x: (Constants.GridWidth * 2) - 16
                y: -8
                width: 8
                height: 8
                source: "../images/corner-menu-upright.png"
            }

            BorderImage {
                id: border_image4
                x: (Constants.GridWidth * 2) - 16
                y: 0
                width: 8
                height: (Constants.GridHeight /2) - 8
                source: "../images/border-menu.png"
            }

            BorderImage {
                id: border_image3
                x: -8
                y: -8
                width: 8
                height: 9
                source: "../images/corner-menu-upleft.png"
            }

            BorderImage {
                id: border_image2
                x: -8
                y: 0
                width: 8
                height: (Constants.GridHeight /2) - 8
                source: "../images/border-menu.png"
            }

            BorderImage {
                id: border_image6
                x: 0
                y: -8
                width: (Constants.GridWidth * 2) - 16
                height: 8
                source: "../images/border-menu.png"
            }








        }





        Label {
            x: 176
            y: -17
            height: Constants.GridHeight
            text: "Menu";
            font.pixelSize: Constants.DefaultFontSize
            anchors.verticalCenterOffset: 0
            anchors.horizontalCenterOffset: 0
            anchors.horizontalCenter: mousearea1.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            font.family: "Source Sans Pro"
            color: "white";
        }

        MouseArea {
            id: mousearea1
            anchors.fill: parent;

            property real start: NaN;
            x: 0
            y: 0
            width: 300
            height: 25
            anchors.bottomMargin: 0
            anchors.leftMargin: 0
            anchors.rightMargin: 0
            anchors.topMargin: 0

            onClicked: base.collapsed = !base.collapsed;
            onPositionChanged: {
                var dist = mouse.y - start;
                if( dist > 30 ) {
                    base.collapsed = true;
                } else if( dist < -30 ) {
                    base.collapsed = false;
                }
            }
            onPressed: start = mouse.y;
        }







    }




    states: State {
        name: "collapsed";
        when: base.collapsed;

        PropertyChanges { target: base; height: 0 }
        PropertyChanges { target: shadow; opacity: 0 }
        PropertyChanges { target: background; opacity: 0 }
    }

    transitions: Transition {
        NumberAnimation { duration: 200; properties: "height,opacity"; easing.type: Easing.InOutQuad }
    }
}
