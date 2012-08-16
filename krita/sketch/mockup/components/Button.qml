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

Item {
    id: base;

    signal clicked();

    property bool checkable: false;
    property bool checked: false;

    property alias image: icon.source;
    property color color: "white";
    property alias text: label.text;
    property alias textColor: label.color;
    property alias textSize: label.font.pixelSize;
    property bool shadow: true;

    property bool highlight: true;
    property color highlightColor: color;

    width: Constants.GridWidth;
    height: Constants.GridHeight;

    Rectangle {
        id: fill;
        x: 5
        y: 5
        color: "#00000000"
        anchors.fill: parent;
        anchors.margins: 0;

        visible: true

        Image {
            id: icon;
            anchors.rightMargin: 8
            anchors.leftMargin: 8
            anchors.bottomMargin: 8
            anchors.topMargin: 8
            anchors.fill: parent;
            anchors.margins: Constants.DefaultMargin;
            fillMode: Image.PreserveAspectFit;
            smooth: true;
        }

        Label {
            id: label;
            anchors.centerIn: parent;
        }
    }

    MouseArea {
        id: mouse;
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        anchors.bottomMargin: 0
        anchors.top: fill.bottom
        anchors.right: fill.left
        anchors.bottom: fill.top
        anchors.left: fill.right
        anchors.topMargin: 0
        onClicked: { base.clicked(); if( base.checkable ) base.checked = !base.checked; }
    }


    states: State {
        name: "pressed";
        when: mouse.pressed || base.checked;


        PropertyChanges {
            target: icon
            opacity: 0.600
        }

        PropertyChanges {
            target: mouse
            anchors.topMargin: 0
        }
    }

    transitions: Transition {
        ParallelAnimation {
            NumberAnimation { properties: "size"; duration: 50; }
        }
    }
}
