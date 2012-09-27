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

Item {
    id: base;
    anchors.fill: parent;
    property QtObject layersModel: undefined;
    // tile goes here

    CompositeOpModel {
        id: compositeOpModel;
        view: sketchView.view;
    }

    PanelTextField {
        id: layerName;
        anchors {
            top: parent.top;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        placeholder: "Name";
        text: layersModel ? layersModel.activeName : "";
    }
    ExpandingListView {
        id: compositeOpList;
        anchors {
            top: layerName.bottom;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        model: compositeOpModel; // composite ops list
        currentIndex: layersModel ? layersModel.activeCompositeOp : 0;
        onNewIndex: if(layersModel) layersModel.activeCompositeOp = currentIndex;
    }
    RangeInput {
        id: opacitySlider;
        anchors {
            top: compositeOpList.bottom;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        placeholder: "Opacity"
        min: 0; max: 255; decimals: 0;
        value: layersModel ? layersModel.activeOpacity : 0;
        onValueChanged: if(layersModel) layersModel.activeOpacity = value;
    }
    Row {
        id: visibleAndLockRow;
        anchors {
            top: opacitySlider.bottom;
            horizontalCenter: parent.horizontalCenter;
            margins: Constants.DefaultMargin;
        }
        height: childrenRect.height;
        width: childrenRect.width;
        Button {
            id: visibilityButton
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: (layersModel && layersModel.activeVisible) ? "../images/svg/icon-visible_on-red.svg" : "../images/svg/icon-visible_off-red.svg";
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: if(layersModel) layersModel.activeVisible = !layersModel.activeVisible;
        }
        Button {
            id: lockstateButton
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: (layersModel && layersModel.activeLocked) ? "../images/svg/icon-locked_on-red.svg" : "../images/svg/icon-locked_off-red.svg";
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: if(layersModel) layersModel.activeLocked = !layersModel.activeLocked;
        }
    }
    Label {
        id: channelsLabel;
        anchors {
            top: visibleAndLockRow.bottom;
            left: parent.left;
            leftMargin: Constants.DefaultMargin;
        }
        horizontalAlignment: Text.AlignLeft;
        font.pixelSize: Constants.LargeFontSize;
        font.bold: true;
        height: Constants.GridHeight / 2;
        text: "Active Channels:";
    }
    Row {
        id: channelsRow;
        anchors {
            top: channelsLabel.bottom;
            horizontalCenter: parent.horizontalCenter;
            margins: Constants.DefaultMargin;
        }
        height: childrenRect.height;
        width: childrenRect.width;
        spacing: Constants.DefaultMargin;
        Button {
            id: redChannel
            height: Constants.DefaultFontSize + Constants.DefaultMargin * 2;
            width: height;
            text: "R";
            textColor: "gray";
            bold: true;
            border { width: 1; color: "silver"; }
            radius: Constants.DefaultMargin;
            color: (layersModel && layersModel.activeRChannelActive) ? "#F0E4E7" : "transparent";
            onClicked: if(layersModel) layersModel.activeRChannelActive = !layersModel.activeRChannelActive;
        }
        Button {
            id: greenChannel
            height: Constants.DefaultFontSize + Constants.DefaultMargin * 2;
            width: height;
            text: "G";
            textColor: "gray";
            bold: true;
            border { width: 1; color: "silver"; }
            radius: Constants.DefaultMargin;
            color: (layersModel && layersModel.activeGChannelActive) ? "#F0E4E7" : "transparent";
            onClicked: if(layersModel) layersModel.activeGChannelActive = !layersModel.activeGChannelActive;
        }
        Button {
            id: blueChannel
            height: Constants.DefaultFontSize + Constants.DefaultMargin * 2;
            width: height;
            text: "B";
            textColor: "gray";
            bold: true;
            border { width: 1; color: "silver"; }
            radius: Constants.DefaultMargin;
            color: (layersModel && layersModel.activeBChannelActive) ? "#F0E4E7" : "transparent";
            onClicked: if(layersModel) layersModel.activeBChannelActive = !layersModel.activeBChannelActive;
        }
        Button {
            id: alphaChannel
            height: Constants.DefaultFontSize + Constants.DefaultMargin * 2;
            width: height;
            text: "A";
            textColor: "gray";
            bold: true;
            border { width: 1; color: "silver"; }
            radius: Constants.DefaultMargin;
            color: (layersModel && layersModel.activeAChannelActive) ? "#F0E4E7" : "transparent";
            onClicked: if(layersModel) layersModel.activeAChannelActive = !layersModel.activeAChannelActive;
        }
    }
    Label {
        id: channelsLockedLabel;
        anchors {
            top: channelsRow.bottom;
            left: parent.left;
            leftMargin: Constants.DefaultMargin;
        }
        horizontalAlignment: Text.AlignLeft;
        font.pixelSize: Constants.LargeFontSize;
        font.bold: true;
        height: Constants.GridHeight / 2;
        text: "Active Channels:";
    }
    Row {
        id: lockedChannelsRow;
        anchors {
            top: channelsLockedLabel.bottom;
            horizontalCenter: parent.horizontalCenter;
            margins: Constants.DefaultMargin;
        }
        height: childrenRect.height;
        width: childrenRect.width;
        spacing: Constants.DefaultMargin;
        Button {
            id: redLockedChannel
            height: Constants.DefaultFontSize + Constants.DefaultMargin * 2;
            width: height;
            text: "R";
            textColor: "gray";
            bold: true;
            border { width: 1; color: "silver"; }
            radius: Constants.DefaultMargin;
            color: (layersModel && layersModel.activeRChannelLocked) ? "#F0E4E7" : "transparent";
            onClicked: if(layersModel) layersModel.activeRChannelLocked = !layersModel.activeRChannelLocked;
        }
        Button {
            id: greenLockedChannel
            height: Constants.DefaultFontSize + Constants.DefaultMargin * 2;
            width: height;
            text: "G";
            textColor: "gray";
            bold: true;
            border { width: 1; color: "silver"; }
            radius: Constants.DefaultMargin;
            color: (layersModel && layersModel.activeGChannelLocked) ? "#F0E4E7" : "transparent";
            onClicked: if(layersModel) layersModel.activeGChannelLocked = !layersModel.activeGChannelLocked;
        }
        Button {
            id: blueLockedChannel
            height: Constants.DefaultFontSize + Constants.DefaultMargin * 2;
            width: height;
            text: "B";
            textColor: "gray";
            bold: true;
            border { width: 1; color: "silver"; }
            radius: Constants.DefaultMargin;
            color: (layersModel && layersModel.activeBChannelLocked) ? "#F0E4E7" : "transparent";
            onClicked: if(layersModel) layersModel.activeBChannelLocked = !layersModel.activeBChannelLocked;
        }
        Button {
            id: alphaLockedChannel
            height: Constants.DefaultFontSize + Constants.DefaultMargin * 2;
            width: height;
            text: "A";
            textColor: "gray";
            bold: true;
            border { width: 1; color: "silver"; }
            radius: Constants.DefaultMargin;
            color: (layersModel && layersModel.activeAChannelLocked) ? "#F0E4E7" : "transparent";
            onClicked: if(layersModel) layersModel.activeAChannelLocked = !layersModel.activeAChannelLocked;
        }
    }
}