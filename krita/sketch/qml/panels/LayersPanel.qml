/* This file is part of the KDE project
 * Copyright (C) 2012 Arjen Hiemstra <ahiemstra@heimr.nl>
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

import QtQuick 1.1
import "../components"
import org.krita.sketch 1.0

Panel {
    name: "Layers";
    panelColor: Constants.Theme.SecondaryColor;

    /*LayerModel {
        id: layerModel;
        view: sketchView.view;
    }

    peekContents: ListView {
        anchors.fill: parent;
        model: layerModel;
        delegate: Rectangle {
            color: "#1e0058";
            width: parent.width;
            height: Constants.GridHeight;
            radius: 8

            Label {
                anchors.centerIn: parent;

                text: model.name;
                color: "white";

                font.pixelSize: Constants.DefaultFontSize;
            }
        }
    }*/

    dragDelegate: Component {
        Rectangle {
            color: Constants.Theme.SecondaryColor;
            width: Constants.GridWidth;
            height: Constants.GridHeight / 2;
            radius: 8

            Label {
                anchors.centerIn: parent;

                text: "Layers";
                color: "white";

                font.pixelSize: Constants.DefaultFontSize;
            }
        }
    }
}
