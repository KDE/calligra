/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2012 Shantanu Tushar <shaan7in@gmail.com>
 * Copyright (C) 2012 Sujith Haridasan <sujith.h@gmail.com>
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
import org.kde.plasma.components 0.1 as PlasmaComponents
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.qtextracomponents 0.1

Item {
    id: findToolbarRootItem
    property QtObject documentController

    Row {
        id: row
        anchors.fill: parent

        PlasmaComponents.TextField {
            id: findToolbarSearchString
            focus: true
            height: parent.height
            width: parent.width - findNextButton.width - findPreviousButton.width

            MouseArea {
                anchors.fill: parent
                onClicked: findToolbarSearchString.forceActiveFocus()
            }

            onTextChanged: findToolbarRootItem.documentController.documentHandler.searchString = text
        }

        PlasmaComponents.Button {
            id: findPreviousButton
            height: parent.height
            width: 128
            text: i18n("Find Previous")

            onClicked: findToolbarRootItem.documentController.documentHandler.findPrevious()
        }

        PlasmaComponents.Button {
            id: findNextButton
            height: parent.height
            width: 128
            text: i18n("Find Next")

            onClicked: findToolbarRootItem.documentController.documentHandler.findNext()
        }
    }
}
