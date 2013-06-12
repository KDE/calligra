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
    property bool isInitialised: false;
    property bool isShown: false;
    onIsShownChanged: {
        if(!isShown)
            return;
        var filterConfig = layerModel.activeFilterConfig;
        if(filterConfig !== null) {
            var categoryIndex = filtersCategoryModel.categoryIndexForConfig(filterConfig);
            var filterIndex = filtersCategoryModel.filterIndexForConfig(categoryIndex, filterConfig);
            filtersCategoryList.currentIndex = categoryIndex;
            fullFilters.currentIndex = filterIndex;
            if(configLoader.item && typeof(configLoader.item.configuration) !== "undefined") {
                configLoader.item.configuration = filterConfig;
            }
        }
        base.isInitialised = (layerModel !== null);
    }
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
        text: layerModel ? layerModel.activeName : "";
        onAccepted: if(layerModel) layerModel.activeName = text;
        onFocusLost: if(layerModel) layerModel.activeName = text;
    }
    ExpandingListView {
        id: compositeOpList;
        anchors {
            top: layerName.bottom;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        visible: layerModel ? (layerModel.activeType === "KisPaintLayer") : false;
        height: visible ? Constants.GridHeight / 2 : 0;
        model: compositeOpModel; // composite ops list
        currentIndex: layerModel ? layerModel.activeCompositeOp : 0;
        onNewIndex: if(layerModel) layerModel.activeCompositeOp = currentIndex;
    }

    FiltersCategoryModel {
        id: filtersCategoryModel;
        view: sketchView.view;
    }
    ExpandingListView {
        id: filtersCategoryList;
        anchors {
            top: compositeOpList.bottom;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        visible: layerModel ? (layerModel.activeType === "KisFilterMask" || layerModel.activeType === "KisAdjustmentLayer") : false;
        height: visible ? Constants.GridHeight / 2 : 0;
        model: filtersCategoryModel;
        onModelChanged: currentIndex = 0;
        onCurrentIndexChanged: {
            model.activateItem(currentIndex)
            if(base.isInitialised) {
                fullFilters.currentIndex = 0;
            }
        }
    }
    ExpandingListView {
        id: fullFilters;
        anchors {
            top: filtersCategoryList.bottom;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        visible: layerModel ? (layerModel.activeType === "KisFilterMask" || layerModel.activeType === "KisAdjustmentLayer") : false;
        height: visible ? Constants.GridHeight / 2 : 0;
        model: filtersCategoryModel.filterModel;
        function applyConfiguration(configuration) {
            if(base.isInitialised) {
                layerModel.activeFilterConfig = configuration;
            }
        }
        onCurrentIndexChanged: {
            if(layerModel.activeType === "KisFilterMask" || layerModel.activeType === "KisAdjustmentLayer") {
                filtersCategoryModel.filterSelected(currentIndex);
                if(base.isInitialised) {
                    console.debug("Setting new configuration...");
                    layerModel.activeFilterConfig = model.configuration(currentIndex);
                }
                if(model.filterRequiresConfiguration(currentIndex)) {
                    noConfigNeeded.visible = false;
                    configNeeded.visible = true;
                    configLoader.source = "filterconfigpages/" + model.filterID(currentIndex) + ".qml";
                }
                else {
                    noConfigNeeded.visible = true;
                    configNeeded.visible = false;
                }
            }
        }
    }
    RangeInput {
        id: opacitySlider;
        anchors {
            top: fullFilters.bottom;
            left: parent.left;
            right: parent.right;
            margins: Constants.DefaultMargin;
        }
        placeholder: "Opacity"
        min: 0; max: 255; decimals: 0;
        value: layerModel.activeOpacity;
        Connections { target: base; onIsShownChanged: opacitySlider.value = layerModel.activeOpacity; }
        onValueChanged: if(layerModel) layerModel.activeOpacity = value;
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
            image: (layerModel && layerModel.activeVisible) ? "../images/svg/icon-visible_on-black.svg" : "../images/svg/icon-visible_off-black.svg";
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: if(layerModel) layerModel.activeVisible = !layerModel.activeVisible;
        }
        Button {
            id: lockstateButton
            width: height;
            height: Constants.GridHeight
            color: "transparent";
            image: (layerModel && layerModel.activeLocked) ? "../images/svg/icon-locked_on-black.svg" : "../images/svg/icon-locked_off-black.svg";
            textColor: "white";
            shadow: false;
            highlight: false;
            onClicked: if(layerModel) layerModel.activeLocked = !layerModel.activeLocked;
        }
    }
    Label {
        id: channelsLabel;
        anchors {
            top: visibleAndLockRow.bottom;
            topMargin: Constants.DefaultMargin;
            left: parent.left;
            leftMargin: Constants.DefaultMargin;
        }
        visible: layerModel ? (layerModel.activeType === "KisPaintLayer" || layerModel.activeType === "KisGroupLayer") : false;
        horizontalAlignment: Text.AlignLeft;
        font.pixelSize: Constants.DefaultFontSize;
        font.bold: true;
        height: visible ? Constants.GridHeight / 2 : 0;
        text: "Active Channels:";
    }
    Row {
        id: channelsRow;
        anchors {
            top: channelsLabel.bottom;
            horizontalCenter: parent.horizontalCenter;
            margins: Constants.DefaultMargin;
        }
        visible: layerModel ? (layerModel.activeType === "KisPaintLayer" || layerModel.activeType === "KisGroupLayer") : false;
        height: visible ? childrenRect.height : 0;
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
            color: (layerModel && layerModel.activeRChannelActive) ? "#EAEAEA" : "transparent";
            onClicked: if(layerModel) layerModel.activeRChannelActive = !layerModel.activeRChannelActive;
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
            color: (layerModel && layerModel.activeGChannelActive) ? "#EAEAEA" : "transparent";
            onClicked: if(layerModel) layerModel.activeGChannelActive = !layerModel.activeGChannelActive;
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
            color: (layerModel && layerModel.activeBChannelActive) ? "#EAEAEA" : "transparent";
            onClicked: if(layerModel) layerModel.activeBChannelActive = !layerModel.activeBChannelActive;
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
            color: (layerModel && layerModel.activeAChannelActive) ? "#EAEAEA" : "transparent";
            onClicked: if(layerModel) layerModel.activeAChannelActive = !layerModel.activeAChannelActive;
        }
    }
    Label {
        id: channelsLockedLabel;
        anchors {
            top: channelsRow.bottom;
            topMargin: Constants.DefaultMargin;
            left: parent.left;
            leftMargin: Constants.DefaultMargin;
        }
        visible: layerModel ? (layerModel.activeType === "KisPaintLayer" || layerModel.activeType === "KisGroupLayer") : false;
        horizontalAlignment: Text.AlignLeft;
        font.pixelSize: Constants.DefaultFontSize;
        font.bold: true;
        height: visible ? Constants.GridHeight / 2 : 0;
        text: "Locked Channels:";
    }
    Row {
        id: lockedChannelsRow;
        anchors {
            top: channelsLockedLabel.bottom;
            horizontalCenter: parent.horizontalCenter;
            margins: Constants.DefaultMargin;
        }
        visible: layerModel ? (layerModel.activeType === "KisPaintLayer" || layerModel.activeType === "KisGroupLayer") : false;
        height: visible ? childrenRect.height : 0;
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
            color: (layerModel && layerModel.activeRChannelLocked) ? "#EAEAEA" : "transparent";
            onClicked: if(layerModel) layerModel.activeRChannelLocked = !layerModel.activeRChannelLocked;
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
            color: (layerModel && layerModel.activeGChannelLocked) ? "#EAEAEA" : "transparent";
            onClicked: if(layerModel) layerModel.activeGChannelLocked = !layerModel.activeGChannelLocked;
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
            color: (layerModel && layerModel.activeBChannelLocked) ? "#EAEAEA" : "transparent";
            onClicked: if(layerModel) layerModel.activeBChannelLocked = !layerModel.activeBChannelLocked;
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
            color: (layerModel && layerModel.activeAChannelLocked) ? "#EAEAEA" : "transparent";
            onClicked: if(layerModel) layerModel.activeAChannelLocked = !layerModel.activeAChannelLocked;
        }
    }
    Item {
        id: noConfigNeeded;
        visible: layerModel ? (layerModel.activeType === "KisFilterMask" || layerModel.activeType === "KisAdjustmentLayer") : false;
        anchors {
           top: lockedChannelsRow.bottom;
           left: parent.left;
           right: parent.right;
           bottom: parent.bottom;
        }
        Column {
            anchors.fill: parent;
            Item {
                width: parent.width;
                height: Constants.GridHeight;
            }
            Text {
                width: parent.width;
                font.pixelSize: Constants.DefaultFontSize;
                color: Constants.Theme.TextColor;
                font.family: "Source Sans Pro"
                wrapMode: Text.WordWrap;
                horizontalAlignment: Text.AlignHCenter;
                text: "This filter requires no configuration. Click below to apply it.";
            }
            Item {
                width: parent.width;
                height: Constants.GridHeight / 2;
            }
            Button {
                width: height;
                height: Constants.GridHeight
                anchors.horizontalCenter: parent.horizontalCenter;
                color: "transparent";
                image: "../images/svg/icon-apply.svg"
                textColor: "white";
                shadow: false;
                highlight: false;
                onClicked: fullFilters.model.activateFilter(fullFilters.currentIndex);
            }
        }
    }
    Flickable {
        id: configNeeded;
        visible: layerModel ? (layerModel.activeType === "KisFilterMask" || layerModel.activeType === "KisAdjustmentLayer") : false;
        anchors {
           top: lockedChannelsRow.bottom;
           left: parent.left;
           right: parent.right;
           bottom: parent.bottom;
        }
        MouseArea {
            anchors.fill: parent;
            hoverEnabled: true;
            onContainsMouseChanged: configLoader.focus = containsMouse;
        }
        Loader {
            id: configLoader;
            width: parent.width;
            height: item ? item.height : 1;
            onItemChanged: {
                if(item && typeof(item.configuration) !== 'undefined') {
                    item.configuration = layerModel.activeFilterConfig;
                }
            }
        }
    }
}