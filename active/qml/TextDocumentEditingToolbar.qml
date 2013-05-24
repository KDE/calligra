/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
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

import QtQuick 1.1
import org.kde.plasma.components 0.1 as PlasmaComponents

Item
{
    id: root
    property QtObject documentController
    property Item docRootItem

    PlasmaComponents.ToolBar {
        anchors.centerIn: parent
        anchors.fill: parent

        tools: Row {
            anchors.centerIn: parent
            height: parent.height
            spacing: 10

            PlasmaComponents.ToolButton {
                id: editButton
                iconSource: "document-edit"
                height: parent.height
                width: height
                checkable: true

                onClicked: docRootItem.toggleEditing()
            }

            PlasmaComponents.ToolButton {
                iconSource: "edit-copy"
                height: parent.height
                width: height
                visible: editButton.checked

                onClicked: documentController.documentHandler.copy()
            }

            PlasmaComponents.ToolButton {
                id: showOverlayButton
                iconSource: "page-2sides"
                height: parent.height
                width: parent.height

                onClicked: root.parent.parent.toggleOverlay()
            }
        }
    }
}
