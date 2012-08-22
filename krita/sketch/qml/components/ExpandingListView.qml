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
    property alias labelText: label.text;
    property alias model: listView.model;
    property alias currentIndex: listView.currentIndex;
    height: topPart.height;

    Item {
        id: topPart;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        height: topButton.height + (topButton.border.width * 2) + Constants.DefaultMargin;
        Label {
            id: label
            anchors {
                baseline: buttonText.baseline;
                left: parent.left;
            }
        }
        Rectangle {
            id: topButton
            border {
                width: 2;
                color: "silver";
            }
            color: "white";
            opacity: 0.2;
            anchors {
                top: parent.top;
                right: parent.right;
                left: label.right;
            }
            height: Constants.SmallFontSize + (Constants.DefaultMargin * 2);
            radius: height / 2;
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
        Label {
            id: buttonText;
            anchors {
                top: topButton.top;
                right: topButton.right;
                bottom: topButton.bottom;
                left: topButton.left;
                leftMargin: topButton.height / 2;
            }
            text: listView.currentItem.text;
            font.pixelSize: Constants.SmallFontSize;
        }
    }

    ListView {
        id: listView;
        clip: true;
        anchors {
            top: topPart.bottom;
            left: topPart.left;
            right: topPart.right;
            bottom: parent.bottom;
        }
        opacity: 0;
        delegate: Item {
            property alias text: delegateLabel.text
            anchors {
                left: parent.left;
                leftMargin: Constants.DefaultMargin;
                right: parent.right;
                rightMargin: Constants.DefaultMargin;
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
