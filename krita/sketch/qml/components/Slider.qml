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
import org.krita.sketch 1.0

Item {
    id: base;
    height: Constants.DefaultFontSize;
    property double value: 0;
    property bool highPrecision: false;
    onValueChanged: handle.resetHandle();
    Rectangle {
        id: fill;
        anchors.fill: parent;
        border.width: 1;
        border.color: "silver";
        color: "#63ffffff";
        radius: height / 2;
    }
    Rectangle {
        id: handle;
        function resetHandle()
        {
            // This is required as width is not set if we are not done initialising yet.
            if(base.width === 0)
                return;
            var newX = base.value / 100 * (mouseArea.drag.maximumX - mouseArea.drag.minimumX) + mouseArea.drag.minimumX;
            if(newX !== handle.x)
                handle.x = newX;
        }
        y: 2
        x: 2
        onXChanged: {
            if(highPrecision) {
                base.value = ((handle.x - mouseArea.drag.minimumX) * 100) / (mouseArea.drag.maximumX - mouseArea.drag.minimumX);
            }
            else {
                base.value = Math.round( ((handle.x - mouseArea.drag.minimumX) * 100) / (mouseArea.drag.maximumX - mouseArea.drag.minimumX) );
            }
        }
        height: parent.height - 4;
        width: height;
        radius: (height / 2) + 1;
        color: "silver"
        MouseArea {
            id: mouseArea;
            anchors.fill: parent;
            drag {
                target: parent;
                axis: "XAxis";
                minimumX: 2;
                maximumX: base.width - handle.width - 2;
                onMaximumXChanged: handle.resetHandle();
            }
        }
    }
}
