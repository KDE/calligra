/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <jhahoneyk@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

import QtQuick 1.0
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets

Item {
    property string searchString: findToolbarSearchString.text

    id: findToolbarRootItem
    focus: true

    Row {
        id: row
        anchors.fill: parent

        PlasmaWidgets.LineEdit {
            id: findToolbarSearchString
            height: parent.height
            width: parent.width - findButton.width
        }

        PlasmaWidgets.PushButton {
            id: findButton
            height: parent.height
            width: 128
        }
    }
}
