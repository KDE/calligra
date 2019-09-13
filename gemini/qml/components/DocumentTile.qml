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

import QtQuick 2.0
import QtQuick.Controls 2.2 as QtControls
import org.calligra 1.0

Item {
    id: component;
    signal clicked();
    property string filePath;
    property string imageUrl: "image://recentimage/" + component.filePath;
    property string title;
    Rectangle {
        x: documentImage.x - Constants.DefaultMargin + (documentImage.width - documentImage.paintedWidth) / 2;
        y: documentImage.y - Constants.DefaultMargin + (documentImage.height - documentImage.paintedHeight) / 2;
        width: documentImage.paintedWidth + Constants.DefaultMargin * 2;
        height: documentImage.paintedHeight + Constants.DefaultMargin * 2;
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
            margins: Constants.DefaultMargin / 2;
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
    QtControls.Label {
        id: lblName;
        anchors {
            left: parent.left;
            right: parent.right;
            bottom: parent.bottom;
            margins: Constants.DefaultMargin;
            bottomMargin: Constants.DefaultMargin * 2;
        }
        height: font.pixelSize + Constants.DefaultMargin * 2;
        horizontalAlignment: Text.AlignHCenter;
        verticalAlignment: Text.AlignVCenter;
        text: component.title;
    }
    MouseArea {
        anchors.fill: parent;
        onClicked: component.clicked();
    }
}
