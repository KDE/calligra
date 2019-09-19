/* This file is part of the KDE project
 * Copyright (C) 2019 Dan Leinir Turthra Jensen <admin@leinir.dk>
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

import QtQuick 2.11
import QtQuick.Controls 2.11 as QtControls
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0

Item {
    id: component;
    signal clicked();
    property string filePath;
    property string imageUrl: "image://recentimage/" + component.filePath;
    property string title;
    property alias enabled: mouseArea.enabled;
    width: GridView.view.cellWidth; height: GridView.view.cellHeight
    Item {
        anchors {
            fill: parent;
            margins: Kirigami.Units.largeSpacing * 2
        }
        Rectangle {
            x: documentImage.x - Kirigami.Units.smallSpacing + (documentImage.width - documentImage.paintedWidth) / 2;
            y: documentImage.y - Kirigami.Units.smallSpacing + (documentImage.height - documentImage.paintedHeight) / 2;
            width: documentImage.paintedWidth + Kirigami.Units.smallSpacing * 2;
            height: documentImage.paintedHeight + Kirigami.Units.smallSpacing * 2;
            border {
                color: "silver";
                width: 1;
            }
        }
        Image {
            id: documentImage;
            source: component.imageUrl;
            anchors {
                top: parent.top;
                left: parent.left;
                right: parent.right;
                margins: Kirigami.Units.largeSpacing;
            }
            height: parent.width;
            fillMode: Image.PreserveAspectFit;
            smooth: true;
            asynchronous: true;
            QtControls.BusyIndicator {
                anchors.centerIn: parent
                width: docList.cellWidth / 3
                height: width
                running: parent.status === Image.Loading
            }
        }
    }
    QtControls.Label {
        id: lblName;
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
        }
        height: font.pixelSize + Kirigami.Units.largeSpacing * 2;
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        text: component.title;
    }
    MouseArea {
        id: mouseArea
        anchors.fill: parent;
        onClicked: component.clicked();
    }
}
