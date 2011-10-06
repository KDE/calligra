/*
 *   Copyright 2011 Sebastian Kügler <sebas@kde.org>
 *   Copyright 2011 Shantanu Tushar <jhahoneyk@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

import QtQuick 1.0
import org.kde.plasma.graphicswidgets 0.1 as PlasmaWidgets
import org.kde.plasma.core 0.1 as PlasmaCore
import org.kde.plasma.graphicslayouts 4.7 as GraphicsLayouts

Item {
    PlasmaCore.Theme {
        id: theme
    }

    anchors.margins: 20

    Button {
        id: back
        drawBackground: false
        imageSource: "qrc:///images/go-previous.png"
        anchors.left: parent.left
        anchors.top: parent.top
        height: 64
        width: 64
        z: 2

        onClicked: homeScreen.state = ""
    }

    Column {
        anchors.fill: parent
        anchors.topMargin: 20
        spacing: 80

        Image {
            id: calligra_logo
            source: "qrc:///images/calligra-logo.png"
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Text {
            id: description
            width: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            wrapMode: Text.WordWrap
            text: "<p>Calligra Active 1.0 Beta using Calligra Engine 2.4</p> \
                   <p>Calligra Active is a office document viewer for touch based tablets especially tailored for the Plasma Active platform.</p> \
                   <p><a href=\"http://www.calligra-suite.org/\">http://www.calligra-suite.org</a></p>"
            color: theme.textColor
            styleColor: theme.backgroundColor
            font.pointSize: 20
        }
    }
}
