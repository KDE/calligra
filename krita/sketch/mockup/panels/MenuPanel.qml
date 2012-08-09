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
import ":/components"

Item {
    id: base;

    property bool collapsed: true;

    property alias newButtonChecked: newButton.checked;
    property alias openButtonChecked: openButton.checked;

    signal buttonClicked( string button );

    height: Constants.GridHeight;

    DropShadow {
        id: shadow;
        anchors.fill: parent;
    }

    Rectangle {
        id: background;
        anchors.fill: parent;
        color: Constants.Theme.MainColor;

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

        width: Constants.GridWidth * 2;
        height: Constants.GridHeight / 2;

        DropShadow {
            anchors.fill: parent;

            Rectangle {
                anchors.fill: parent;
                color: Constants.Theme.MainColor;

                Label {
                    anchors.centerIn: parent;
                    text: "Menu";
                    color: "white";
                }
            }
        }

        MouseArea {
            anchors.fill: parent;

            property real start: NaN;

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
