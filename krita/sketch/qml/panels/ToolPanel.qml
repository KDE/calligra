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
import "../components"

Panel {
    name: "Tool";
    panelColor: "#660066";

    dragDelegate: Component {
        Rectangle {
            color: "#660066";
            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
            radius: 8;

            Label {
                anchors.centerIn: parent;

                text: "Tool";
                color: "white";

                font.pixelSize: Constants.DefaultFontSize;
            }
        }
    }
}
