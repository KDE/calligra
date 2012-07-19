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

Rectangle {

    property alias text: title.text;
    property alias leftArea: left.children;
    property alias rightArea: right.children;

    height: Constants.GridHeight;
    width: Constants.GridWidth * Constants.GridColumns;

    color: Constants.Theme.MainColor;

    Row {
        id: left;

        anchors.left: parent.left;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
    }

    Label {
        id: title;
        anchors.centerIn: parent;

        font.pixelSize: Constants.HugeFontSize;
        font.bold: true;
        color: "white";
    }

    Row {
        id: right;

        anchors.right: parent.right;
        anchors.top: parent.top;
        anchors.bottom: parent.bottom;
    }
}