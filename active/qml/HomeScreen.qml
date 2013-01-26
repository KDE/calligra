/*
 * This file is part of the KDE project
 *
 * Copyright (C) 2011 Shantanu Tushar <shaan7in@gmail.com>
 * Copyright (C) 2012 Sujith H <sujith.h@gmail.com>
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
import CalligraActive 1.0
import org.kde.plasma.components 0.1 as PlasmaComponents

Image {
    id: homeScreen

    source: "qrc:///images/fabrictexture.png"
    fillMode: Image.Tile

    PlasmaComponents.Button {
        id: openFileDialogButton

        iconSource: "document-open"
        text: i18n("Open File")
        width: 512
        height: 64

        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 10
        }

        onClicked: mainwindow.openFileDialog()
    }

    PlasmaComponents.BusyIndicator {
        id: loadingIndicator
        anchors {
            horizontalCenter: parent.horizontalCenter
            bottom: parent.bottom
            margins: 20
        }
        visible: !openFileDialogButton.visible
        running: visible
    }

    AboutCalligraActive {
        anchors {
            left: parent.left; right: parent.right
            top: parent.top; bottom: openFileDialogButton.top
            margins: 10
        }
    }

    function hideOpenButton()
    {
        openFileDialogButton.visible = false
    }
}
