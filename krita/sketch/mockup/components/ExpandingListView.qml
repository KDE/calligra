/* This file is part of the KDE project
 * Copyright (C) 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

Item {
    id: base
    property alias model: listView.model;
    property alias currentIndex: listView.currentIndex;

    Rectangle {
        id: topButton
        border {
            width: 2;
            color: "gray";
        }
        radius: 5;
        anchors {
            top: parent.top;
            right: parent.right;
            left: parent.left;
        }
        height: Constants.GridHeight;
        Label {
            id: buttonText;
            text: listView.currentItem.text;
        }

        MouseArea {
            anchors.fill: parent;
            onClicked: {
                if(base.state === "expanded") {
                    base.state = "";
                }
                else {
                    base.state = "expanded";
                }
            }
        }
    }

    ListView {
        id: listView;
        anchors {
            top: topButton.bottom;
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        opacity: 0;
        delegate: Item {
            property alias text: delegateLabel.text
            anchors {
                left: parent.left;
                right: parent.right;
            }
            height: Constants.DefaultFontSize + Constants.DefaultMargin * 2;
            Label {
                id: delegateLabel
                anchors.fill: parent
                text: model.text;
            }
            MouseArea {
                anchors.fill: parent;
                onClicked: {
                    listView.currentIndex = index;
                    base.state = "";
                }
            }
        }
    }

    states: [
        State {
            name: "expanded";
            PropertyChanges {
                target: base;
                height: base.parent.height - base.y - (Constants.GridHeight / 2);
            }
            PropertyChanges {
                target: listView;
                opacity: 1;
            }
        }
    ]
    transitions: [
        Transition {
            from: ""
            to: "expanded"
            reversible: true;
            PropertyAnimation { properties: "height,opacity"; duration: 200; easing.type: Easing.InOutCubic }
        }
    ]
}
