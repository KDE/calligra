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
import "components"
import "panels"

Page {
    Rectangle {
        anchors.fill: parent;
        color: "grey"

        DropShadow {
            anchors.centerIn: parent;
            width: parent.width / 2;
            height: parent.height / 2;

            Rectangle {
                anchors.fill: parent;
                color: "white";

                MouseArea {
                    anchors.fill: parent;
                    onClicked: pageStack.pop();
                }
            }
        }

        MenuPanel {
            anchors.bottom: parent.bottom;

            width: parent.width;
            z: 10;

            onButtonClicked: {
                switch( button ) {
                    case "new":
                        newPanel.collapsed = !newPanel.collapsed;
                        break;
                    case "settings":
                        pageStack.push( settingsPage );
                        break;
                    case "share":
                        pageStack.push( sharePage );
                        break;
                }
            }
        }

        CreateNewPanel {
            id: newPanel;
            anchors.left: parent.left;
            width: Constants.GridWidth * 4;
            height: Constants.GridHeight * 7;
        }
    }

    Component { id: sharePage; SharePage { } }
    Component { id: settingsPage; SettingsPage { } }
}