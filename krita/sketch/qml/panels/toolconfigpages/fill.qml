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
    Connections {
        target: compositeOpModel;
        onOpacityChanged: opacityInput.value = compositeOpModel.opacity;
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

        RangeInput {
            id: opacityInput;
            width: parent.width;
            placeholder: "Opacity";
            min: 0; max: 1; decimals: 2;
            value: compositeOpModel.opacity;
            onValueChanged: compositeOpModel.changePaintopValue("opacity", value);
            enabled: compositeOpModel.opacityEnabled;
        }

        Item {
            width: parent.width;
            height: Constants.DefaultMargin;
        }

        RangeInput {
            id: thresholdInput;
            width: parent.width;
            placeholder: "Threshold";
            min: 0; max: 255; decimals: 0;
            value: 255;
            onValueChanged: if(toolManager.currentTool) toolManager.currentTool.slotSetThreshold(value);
        }

        CheckBox {
            id: fillSelectionCheck;
            anchors {
                left: parent.left;
                right: parent.right;
                margins: Constants.DefaultMargin;
            }
            text: "Fill Selection";
        }
        CheckBox {
            id: limitToLayerCheck;
            anchors {
                left: parent.left;
                right: parent.right;
                margins: Constants.DefaultMargin;
            }
            text: "Limit to Layer";
        }
    }
}
