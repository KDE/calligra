/* This file is part of the KDE project
 * Copyright (C) 2012 Dan Leinir Turthra Jensen <admin@leinir.dk>
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
import org.krita.sketch 1.0
import "../../components"

Item {
    id: base
    property bool fullView: true;
    ExpandingListView {
        id: compositeModeList
        visible: fullView;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        onCurrentIndexChanged: model.activateItem(currentIndex);
        model: compositeOpModel;
    }
    Column {
        anchors {
            top: fullView ? compositeModeList.bottom : compositeModeList.top;
            left: parent.left;
            leftMargin: Constants.DefaultMargin;
            right: parent.right;
            rightMargin: Constants.DefaultMargin;
        }

        PanelTextField {
            width: parent.width;
            placeholder: "Size";
            text: "5.00";
        }
        PanelTextField {
            width: parent.width;
            placeholder: "Opacity";
            text: "1.00";
        }
        PanelTextField {
            width: parent.width;
            placeholder: "Flow";
            text: "1.00";
        }

        PanelTextField {
            visible: fullView;
            width: parent.width;
            placeholder: "Smoothness";
            text: "100%";
        }
        PanelTextField {
            visible: fullView;
            width: parent.width;
            placeholder: "Assistant";
            text: "100%";
        }
    
        Label {
            visible: fullView;
            width: parent.width;
            horizontalAlignment: Text.AlignLeft;
            font.pixelSize: Constants.LargeFontSize;
            font.bold: true;
            height: Constants.GridHeight / 2;
            text: "Mirror:";
        }

        Item {
            width: childrenRect.width;
            height: childrenRect.height;
            anchors.horizontalCenter: parent.horizontalCenter;
            Button {
                id: mirrorVertical;
                image: "../../images/svg/icon-mirror_v-blue.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
            Button {
                id: mirrorHorizontal;
                anchors.left: mirrorVertical.right;
                image: "../../images/svg/icon-mirror_h-blue.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
            Button {
                id: mirrorCenter;
                anchors.left: mirrorHorizontal.right;
                image: "../../images/svg/icon-mirror_c-blue.svg"
                width: Constants.GridWidth * 2 / 5;
                height: width;
                color: "transparent";
                shadow: false;
                highlight: false;
            }
        }
    }
}
