/* This file is part of the KDE project
 * Copyright (C) 2014 Stuart Dickson <stuartmd@kogmbh.com>
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

Item {
    id: base

    property color freeColor: "lightblue"
    property color usedColor: "blue"
    property color oldItemColor: "#dbf6ff"
    property color newItemColor: "yellow"
    property color excessColor: "red"

    property real maxCapacity: 100
    property real usedCapacity: 50
    property real oldItemSize: 10
    property real newItemSize: 10

    QtObject {
        id: d;
        property real finalUsedCapacity: base.usedCapacity + base.newItemSize - base.oldItemSize;
        property bool isOverCapacity: finalUsedCapacity > base.maxCapacity;
        property real currentFreeSpaceWidth: ((base.maxCapacity - base.usedCapacity) / base.maxCapacity) * completeSpace.width;
        property real newFreeSpaceWidth: isOverCapacity ? 0 : ((base.maxCapacity - finalUsedCapacity) / base.maxCapacity) * completeSpace.width;
        property real oldItemWidth: (oldItemSize / base.maxCapacity) * completeSpace.width;
    }

    Rectangle {
        id: completeSpace;
        anchors.fill: parent;
        anchors.rightMargin: parent.height; // space for excess indicator
        color: base.freeColor;

        Rectangle {
            id: usedSpace;
            anchors {
                fill: parent;
                rightMargin: d.currentFreeSpaceWidth;
            }
            color: base.usedColor;
        }

        Rectangle {
            id: oldItem;
            anchors {
                right: parent.right;
                top: parent.top;
                bottom: parent.bottom;
                rightMargin: d.currentFreeSpaceWidth;
            }
            width: d.oldItemWidth;
            color: base.oldItemColor;
        }

        Rectangle {
            id: newItem;
            anchors {
                right: parent.right;
                left: oldItem.left;
                top: parent.top;
                bottom: parent.bottom;
                rightMargin: d.newFreeSpaceWidth;
            }
            color: (d.isOverCapacity) ? base.excessColor : base.newItemColor;
        }
    }

    Item {
        id: excessDecoration;
        visible: d.isOverCapacity;
        anchors {
            left: completeSpace.right;
            right: parent.right;
            top: parent.top;
            bottom: parent.bottom;
        }
        Rectangle {
            anchors.centerIn: parent;
            width:parent.height;
            height:parent.width;
            rotation: -90;
            gradient: Gradient {
                GradientStop { position: 0.0; color: base.excessColor; }
                GradientStop { position: 0.2; color: base.excessColor; }
                GradientStop { position: 0.8; color: Qt.rgba(base.excessColor.r, base.excessColor.g, base.excessColor.b, 0.0); }
                GradientStop { position: 1.0; color: Qt.rgba(base.excessColor.r, base.excessColor.g, base.excessColor.b, 0.0); }
            }
        }
    }
}
