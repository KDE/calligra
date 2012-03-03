/*
 * Copyright 2012  Yue Liu <yue.liu@mail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA  02110-1301  USA
 */

import QtQuick 1.1

Rectangle {
    width: 300
    height: 500

    ListModel {
        id: appModel
        ListElement { name: "Stencil 1"; Item { ListModel {} } }
    }

    ListModel {
        id: stencilModel
        ListElement { name: "Music"; icon: "tmp.png" }
        ListElement { name: "Movies"; icon: "tmp.png" }
        ListElement { name: "Camera"; icon: "tmp.png" }
        ListElement { name: "Calendar"; icon: "tmp.png" }
        ListElement { name: "Messaging"; icon: "tmp.png" }
        ListElement { name: "Todo List"; icon: "tmp.png" }
        ListElement { name: "Contacts"; icon: "tmp.png" }
        ListElement { name: "Music"; icon: "tmp.png" }
        ListElement { name: "Movies"; icon: "tmp.png" }
        ListElement { name: "Camera"; icon: "tmp.png" }
        ListElement { name: "Calendar"; icon: "tmp.png" }
        ListElement { name: "Messaging"; icon: "tmp.png" }
        ListElement { name: "Todo List"; icon: "tmp.png" }
        ListElement { name: "Contacts"; icon: "tmp.png" }
    }

    Component {
        id: stencilDelegate
        Item {
            anchors.fill: parent
            Column {
                StencilHeader {}
                StencilGridView {}
            }
        }
    }

    Column {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.verticalCenter: parent.verticalCenter

        spacing: 2
        highlight: Rectangle { color: "lightsteelblue"; radius: 5 }
        focus: true
    }
}
