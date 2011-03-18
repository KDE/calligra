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

import QtQuick 1.0 as QML

QML.Rectangle {
    property alias text: buttonText.text
    property alias imageSource: buttonImage.source
    
    gradient: QML.Gradient {
         QML.GradientStop { position: 0.0; color: "#DCDCDC" }
         QML.GradientStop { position: 1.0; color: "#ABABAB" }
    }
    
    radius: 10

    QML.Column {
        anchors.fill: parent
        anchors.margins: 10

        QML.Image {
            id: buttonImage
            
            width: parent.width
            height: parent.height*0.75
        }

        QML.Text {
            id: buttonText
            
            color: "white"            
            width: parent.width
            height: parent.height*0.25
            horizontalAlignment: QML.Text.AlignHCenter
            verticalAlignment: QML.Text.AlignVCenter
        }
    }
    
    
}