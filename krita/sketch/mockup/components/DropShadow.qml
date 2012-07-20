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
    property real size: Constants.DefaultMargin * 0.666;

    Image {
        anchors.right: parent.left;
        anchors.bottom: parent.top;

        width: parent.size;
        height: parent.size;

        opacity: 0.5;

        source: ":/images/shadow-corner.png";
    }

    Image {
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.bottom: parent.top;

        width: parent.width;
        height: parent.size;

        opacity: 0.5;

        source: ":/images/shadow-edge-horizontal.png";
    }

    Image {
        anchors.left: parent.right;
        anchors.bottom: parent.top;

        width: parent.size;
        height: parent.size;
        rotation: 90;

        opacity: 0.5;

        source: ":/images/shadow-corner.png";
    }

    Image {
        anchors.left: parent.right;
        anchors.verticalCenter: parent.verticalCenter;

        width: parent.size;
        height: parent.height;
        scale: -1;

        opacity: 0.5;

        source: ":/images/shadow-edge-vertical.png";
    }

    Image {
        anchors.left: parent.right;
        anchors.top: parent.bottom;

        width: parent.size;
        height: parent.size;
        rotation: 180;

        opacity: 0.5;

        source: ":/images/shadow-corner.png";
    }

    Image {
        anchors.horizontalCenter: parent.horizontalCenter;
        anchors.top: parent.bottom;

        width: parent.width;
        height: parent.size;
        scale: -1;

        opacity: 0.5;

        source: ":/images/shadow-edge-horizontal.png";
    }

    Image {
        anchors.right: parent.left;
        anchors.top: parent.bottom;

        width: parent.size;
        height: parent.size;
        rotation: 270;

        opacity: 0.5;

        source: ":/images/shadow-corner.png";
    }

    Image {
        anchors.right: parent.left;
        anchors.verticalCenter: parent.verticalCenter;

        width: parent.size;
        height: parent.height;

        opacity: 0.5;

        source: ":/images/shadow-edge-vertical.png";
    }
}