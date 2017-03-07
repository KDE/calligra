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

import QtQuick 2.0
import org.calligra 1.0

Item {
    id: base;
    property int progress: 0;
    Rectangle {
        id: progressBase;
        anchors.centerIn: parent;
        height: Constants.LargeFontSize + 4;
        width: 208;
        radius: height / 2;
        border {
            width: 1;
            color: Settings.theme.color("components/dialog/progress/border");
        }
        color: Settings.theme.color("components/dialog/progress/background");
        Rectangle {
            id: progressBar;
            anchors {
                top: parent.top;
                left: parent.left;
                margins: 4;
            }
            radius: height / 2;
            width: progress >= 0 ? (progress * 2) + 1: 100;
            height: parent.height - 7;
            Behavior on width { PropertyAnimation { duration: 100; } }
            color: Settings.theme.color("components/dialog/progress/bar");
        }
    }
}
