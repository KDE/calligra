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

Rectangle {
    id: buttonContainer

    property alias text: buttonText.text
    property alias imageSource: buttonImage.source
    property alias textPosition: buttonContainer.state
    signal clicked

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#DCDCDC" }
        GradientStop { position: 1.0; color: "#ABABAB" }
    }
    radius: 10

    Image {
        id: buttonImage

        anchors.horizontalCenter: buttonContainer.horizontalCenter
        width: height
        height: buttonContainer.height - buttonText.height

        MouseArea {
            anchors.fill: parent
            onClicked: buttonContainer.clicked()
        }
    }

    Text {
        id: buttonText

        width: buttonContainer.width
        anchors.top: buttonImage.bottom;
        anchors.horizontalCenter: buttonImage.horizontalCenter
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter

        elide: Text.ElideMiddle

        MouseArea {
            anchors.fill: parent
            onClicked: buttonContainer.clicked()
        }
    }

    states: [
        State {
            name: "right"
            AnchorChanges {
                target: buttonImage
                anchors.verticalCenter: buttonContainer.verticalCenter
                anchors.horizontalCenter: undefined
                anchors.left: buttonContainer.left
            }
            PropertyChanges {
                target: buttonImage
                height: buttonContainer.height
            }
            AnchorChanges {
                target: buttonText
                anchors.verticalCenter: buttonImage.verticalCenter
                anchors.horizontalCenter: undefined
                anchors.left: buttonImage.right
            }
            PropertyChanges {
                target: buttonText
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                width: buttonContainer.width - buttonImage.width
            }
        }
    ]
}
