/* This file is part of the KDE project
 * Copyright (C) 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
    opacity: parent.checked ? 1 : 0;
    Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
    anchors {
        top: parent.bottom;
        right: parent.right;
        topMargin: 4;
    }
    height: Constants.ToolbarHeight * 4;
    width: Constants.GridWidth * 3;
    Rectangle {
        anchors.fill: parent;
        color: "#4e5359";
        opacity: 0.96;
        border.color: "#22282f";
        border.width: 1;
    }
    Column {
        anchors.fill: parent;
        Button {
            opacity: switchToDesktopAction.enabled ? 1 : 0.2;
            Behavior on opacity { PropertyAnimation { duration: Constants.AnimationDuration; } }
            image: Settings.theme.icon("SVG-Icon-DesktopMode-1");
            text: "Switch to Desktop Mode";
            height: Constants.ToolbarHeight;
            width: parent.width;
            onClicked: switchToDesktopAction.trigger();
        }
        Button {
            image: Settings.theme.icon("SVG-Icon-Help-1");
            text: "Help";
            height: Constants.ToolbarHeight;
            width: parent.width;
        }
        Button {
            image: Settings.theme.icon("SVG-Icon-Find-1");
            text: "Find";
            height: Constants.ToolbarHeight;
            width: parent.width;
        }
        Button {
            image: Settings.theme.icon("SVG-Icon-SpellCheck-1");
            text: "Check Spelling";
            height: Constants.ToolbarHeight;
            width: parent.width;
        }
    }
}
