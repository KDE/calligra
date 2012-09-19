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
        height: childrenRect.height;

        PanelTextField {
            width: parent.width;
            placeholder: "Size";
            text: compositeOpModel.size;
            onFocusLost: compositeOpModel.changePaintopValue("size", text);
            onAccepted: compositeOpModel.changePaintopValue("size", text);
            enabled: compositeOpModel.sizeEnabled;
        }
        PanelTextField {
            width: parent.width;
            placeholder: "Opacity";
            text: compositeOpModel.opacity;
            onFocusLost: compositeOpModel.changePaintopValue("opacity", text);
            onAccepted: compositeOpModel.changePaintopValue("opacity", text);
            enabled: compositeOpModel.opacityEnabled;
        }
        PanelTextField {
            width: parent.width;
            placeholder: "Flow";
            text: compositeOpModel.flow;
            onFocusLost: compositeOpModel.changePaintopValue("flow", text);
            onAccepted: compositeOpModel.changePaintopValue("flow", text);
            enabled: compositeOpModel.flowEnabled;
        }

        PanelTextField {
            visible: fullView;
            width: parent.width;
            placeholder: "Smoothness";
            text: "1000";
            onFocusLost: if(toolManager.currentTool) toolManager.currentTool.slotSetSmoothness(text);
            onAccepted: if(toolManager.currentTool) toolManager.currentTool.slotSetSmoothness(text);
        }
        PanelTextField {
            visible: fullView;
            width: parent.width;
            placeholder: "Assistant";
            text: "1000";
            onFocusLost: if(toolManager.currentTool) toolManager.currentTool.slotSetMagnetism(text);
            onAccepted: if(toolManager.currentTool) toolManager.currentTool.slotSetMagnetism(text);
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
                checked: compositeOpModel.mirrorVertically;
                onClicked: compositeOpModel.mirrorVertically = !compositeOpModel.mirrorVertically;
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
                checked: compositeOpModel.mirrorHorizontally;
                onClicked: compositeOpModel.mirrorHorizontally = !compositeOpModel.mirrorHorizontally;
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
                checked: compositeOpModel.mirrorCenter;
                onClicked: compositeOpModel.mirrorCenter = !compositeOpModel.mirrorCenter;
            }
        }
    }
}
