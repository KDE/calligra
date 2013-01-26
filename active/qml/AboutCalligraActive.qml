/*
 *   Copyright 2011 Sebastian Kügler <sebas@kde.org>
 *   Copyright 2011 Shantanu Tushar <shaan7in@gmail.com>
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
import org.kde.plasma.components 0.1 as PlasmaComponents

Item {
    PlasmaCore.Theme {
        id: theme
    }

    Column {
        anchors.fill: parent
        anchors.margins: 40
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
            text: i18n("<p>Calligra Active 1.0 Beta using Calligra Office Engine %1 </p> \
                   <p>Calligra Active is an office document viewer for tablets especially tailored to the Plasma Active platform.</p> \
                   <p><a href=\"http://www.calligra.org/\">http://www.calligra.org</a></p>", _calligra_version_string)
            color: theme.textColor
            styleColor: theme.backgroundColor
            font.pointSize: 20
        }
    }
}
